# Homebrew Formula Plan for zvec

## Goal

Make `gem install zvec` fast by pre-building the zvec C++ library via Homebrew. Today the gem compiles the entire C++ dependency tree from source (~10 minutes). With a Homebrew formula, the gem install only compiles 8 small Rice binding files (~10 seconds).

**End-user experience:**

```bash
brew tap madbomber/zvec https://github.com/MadBomber/zvec-ruby.git
brew install madbomber/zvec/zvec
gem install zvec
```

## Current State

The zvec C++ library has no install target for its core libraries. The build produces ~12 static archives plus vendored thirdparty libraries (Arrow, RocksDB, protobuf, antlr4, glog, CRoaring, lz4). Everything is built from vendored source — the only external dependency is ICU4C.

The Ruby gem's native extension links against:

| Library | Link mode | Purpose |
|---------|-----------|---------|
| `zvec_db` | normal | Top-level database API |
| `core_knn_flat_static` | force-load | Flat index algorithm |
| `core_knn_flat_sparse_static` | force-load | Flat sparse index |
| `core_knn_hnsw_static` | force-load | HNSW algorithm |
| `core_knn_hnsw_sparse_static` | force-load | HNSW sparse |
| `core_knn_ivf_static` | force-load | IVF algorithm |
| `core_knn_cluster_static` | force-load | Clustering |
| `core_mix_reducer_static` | force-load | Mixed reducer |
| `core_metric_static` | force-load | Distance metrics |
| `core_utility_static` | force-load | Utilities |
| `core_quantizer_static` | force-load | Quantization |

The 10 algorithm libraries must be force-loaded (`-Wl,-force_load`) because they use a self-registration pattern — static constructors register algorithm implementations with a factory at load time.

## Strategy: Single Fat Static Library

Rather than installing 12+ separate `.a` files, the formula should produce a **single fat static archive** (`libzvec.a`) that bundles everything:

- All zvec libraries (zvec_db, zvec_core, zvec_ailego, algorithm libs)
- All vendored thirdparty static libraries (Arrow, RocksDB, protobuf, etc.)

This simplifies linking for consumers. The Ruby gem links one library instead of managing a dependency graph.

**The entire library must be force-loaded** since the algorithm registration pattern requires all object files to be included (not just those with referenced symbols).

## Homebrew Formula

### Formula Outline

```ruby
class Zvec < Formula
  desc "Embedded vector database for high-dimensional similarity search"
  homepage "https://github.com/alibaba/zvec"
  url "https://github.com/alibaba/zvec.git",
      tag:      "v0.2.0",
      revision: "232803611ea91abc96904a92c878c4c67ddf73e6"
  license "Apache-2.0"

  depends_on "cmake" => :build
  depends_on "ninja" => :build
  depends_on "icu4c@78"

  def install
    # Set build options
    args = %W[
      -G Ninja
      -DCMAKE_BUILD_TYPE=Release
      -DCMAKE_PREFIX_PATH=#{Formula["icu4c@78"].prefix}
      -DBUILD_PYTHON_BINDINGS=OFF
      -DBUILD_TOOLS=OFF
      -DBUILD_TESTING=OFF
      -DBENCHMARK_ENABLE_TESTING=OFF
      -DBENCHMARK_ENABLE_INSTALL=OFF
      -DPROJECT_ROOT_DIR=#{buildpath}
    ]

    # Configure and build
    system "cmake", "-S", ".", "-B", "build", *args, *std_cmake_args
    system "cmake", "--build", "build"

    # Install headers
    (include/"zvec").install Dir["src/include/zvec/**/*"], preserve: true
    # Alternatively: cp_r "src/include/zvec", include/"zvec"

    # Create fat static library from all zvec + thirdparty archives
    # (see "Creating the Fat Library" below)
    create_fat_library(buildpath/"build", lib)

    # Install a pkg-config file for easy discovery
    (lib/"pkgconfig").mkpath
    (lib/"pkgconfig/zvec.pc").write pkg_config_file
  end

  private

  def create_fat_library(build_dir, lib_dir)
    # Collect all static archives that the Ruby gem needs
    archives = Dir[
      build_dir/"**/*.a"
    ].select { |f| relevant_archive?(f) }

    # Use libtool to merge into a single fat archive
    lib_dir.mkpath
    system "libtool", "-static", "-o", lib_dir/"libzvec.a", *archives
  end

  def relevant_archive?(path)
    # Include zvec's own libraries and vendored thirdparty
    # Exclude test/benchmark archives
    basename = File.basename(path)
    return false if basename.match?(/test|benchmark|gmock|gtest/)
    true
  end

  def pkg_config_file
    <<~PC
      prefix=#{prefix}
      libdir=${prefix}/lib
      includedir=${prefix}/include

      Name: zvec
      Description: Embedded vector database
      Version: 0.2.0
      Libs: -L${libdir} -Wl,-force_load,${libdir}/libzvec.a
      Cflags: -I${includedir}
    PC
  end

  test do
    # Minimal compile test
    (testpath/"test.cpp").write <<~CPP
      #include <zvec/db/collection.h>
      int main() { return 0; }
    CPP
    system ENV.cxx, "-std=c++17", "test.cpp",
           "-I#{include}", "-L#{lib}",
           "-Wl,-force_load,#{lib}/libzvec.a",
           "-o", "test"
  end
end
```

### Creating the Fat Library

The `libtool -static` approach merges all `.a` files into one. On macOS:

```bash
libtool -static -o lib/libzvec.a \
  build/src/db/libzvec_db.a \
  build/src/core/libzvec_core.a \
  build/src/ailego/libzvec_ailego.a \
  build/src/core/algorithm/flat/libcore_knn_flat.a \
  build/src/core/algorithm/hnsw/libcore_knn_hnsw.a \
  ... (all algorithm libs) \
  build/thirdparty/rocksdb/*/librocksdb.a \
  build/thirdparty/arrow/*/libarrow.a \
  build/thirdparty/protobuf/*/libprotobuf.a \
  ... (all thirdparty libs)
```

Using `Dir["build/**/*.a"]` with exclusions for test/benchmark archives is simpler than listing each one.

### What Gets Installed

```
/opt/homebrew/opt/zvec/
  include/
    zvec/
      ailego/...          (utility headers)
      core/
        framework/...     (index framework headers)
        interface/...     (public API: index.h, index_param.h, etc.)
      db/
        collection.h      (main Collection class)
        config.h
        doc.h
        index_params.h
        options.h
        query_params.h
        schema.h
        stats.h
        status.h
        type.h
  lib/
    libzvec.a             (single fat static archive, ~150-200 MB)
    pkgconfig/
      zvec.pc             (pkg-config file for discovery)
```

## Changes to the Ruby Gem

### ext/CMakeLists.txt — Three-Tier Resolution

The CMakeLists.txt should try three strategies in order:

1. **Homebrew zvec** — fast path, link against pre-built `libzvec.a`
2. **Local submodule** — development path, build from `vendor/zvec`
3. **FetchContent** — fallback, download and build from GitHub

```cmake
# --- Strategy 1: Find pre-installed zvec (e.g., from Homebrew) ---
find_library(ZVEC_LIBRARY zvec HINTS /opt/homebrew/opt/zvec/lib)
find_path(ZVEC_INCLUDE_DIR zvec/db/collection.h HINTS /opt/homebrew/opt/zvec/include)

if(ZVEC_LIBRARY AND ZVEC_INCLUDE_DIR)
  message(STATUS "Found pre-installed zvec: ${ZVEC_LIBRARY}")
  set(ZVEC_PREBUILT TRUE)
else()
  set(ZVEC_PREBUILT FALSE)

  # Strategy 2: Local submodule (development)
  set(_ZVEC_LOCAL "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/zvec")
  if(EXISTS "${_ZVEC_LOCAL}/CMakeLists.txt")
    message(STATUS "Using local zvec from ${_ZVEC_LOCAL}")
    set(ZVEC_ROOT "${_ZVEC_LOCAL}")
  else()
    # Strategy 3: FetchContent (gem install without Homebrew)
    message(STATUS "Fetching zvec from GitHub...")
    FetchContent_Declare(zvec_src
      GIT_REPOSITORY https://github.com/alibaba/zvec.git
      GIT_TAG        232803611ea91abc96904a92c878c4c67ddf73e6
      GIT_SHALLOW    FALSE
      GIT_SUBMODULES_RECURSE TRUE
    )
    FetchContent_GetProperties(zvec_src)
    if(NOT zvec_src_POPULATED)
      FetchContent_Populate(zvec_src)
    endif()
    set(ZVEC_ROOT "${zvec_src_SOURCE_DIR}")
  endif()

  # ... (existing add_subdirectory, antlr4 patches, etc.)
endif()

# --- Ruby extension module ---
add_library(zvec_ext MODULE ...)

if(ZVEC_PREBUILT)
  # Link against the single fat archive
  target_include_directories(zvec_ext PRIVATE "${ZVEC_INCLUDE_DIR}")
  target_link_libraries(zvec_ext PRIVATE -Wl,-force_load,${ZVEC_LIBRARY})
else()
  # Link against individual build targets (existing code)
  target_include_directories(zvec_ext PRIVATE "${ZVEC_ROOT}/src/include" "${ZVEC_ROOT}/src")
  foreach(lib ${ZVEC_ALGO_LIBS})
    target_link_libraries(zvec_ext PRIVATE -Wl,-force_load,$<TARGET_FILE:${lib}>)
  endforeach()
  target_link_libraries(zvec_ext PRIVATE zvec_db)
endif()
```

### gemspec Change

Add a post-install message pointing users to the fast path:

```ruby
spec.post_install_message = <<~MSG
  zvec installed successfully!

  Tip: For faster future installs, pre-build the C++ library with Homebrew:
    brew install zvec
MSG
```

## Build Time Comparison

| Scenario | Configure | Build | Total |
|----------|-----------|-------|-------|
| `gem install` (no Homebrew) | ~3 min | ~7 min | ~10 min |
| `brew install zvec` (one-time) | ~3 min | ~7 min | ~10 min |
| `gem install` (with Homebrew zvec) | ~5 sec | ~5 sec | ~10 sec |

## Implementation Steps

### Phase 1: Upstream Prep (Optional)

1. Open a PR on `alibaba/zvec` adding a proper `install()` target for the C++ library and headers
2. This would make the formula cleaner — `cmake --install build` would just work
3. If upstream is unresponsive, the formula handles installation manually

### Phase 2: Homebrew Formula

1. Create the formula in a tap: `homebrew-zvec` or contribute to `homebrew-core`
2. Start with a personal tap (`brew tap madbomber/zvec`) for iteration
3. Test on a clean machine (or GitHub Actions macOS runner)
4. Key challenge: enumerating the correct set of `.a` files for the fat archive

### Phase 3: Gem CMakeLists.txt Changes

1. Add `find_library`/`find_path` detection for pre-installed zvec
2. Add the `ZVEC_PREBUILT` conditional linking path
3. Keep the existing FetchContent fallback for users without Homebrew
4. Test all three paths: Homebrew, local submodule, FetchContent

### Phase 4: Documentation

1. Update README prerequisites to mention `brew install zvec` as recommended
2. Update the docs site installation guide
3. Add troubleshooting for common issues (wrong ICU version, missing Homebrew, etc.)

## Open Questions

1. **Shared vs static library** — A shared `libzvec.dylib` would be smaller (~30 MB vs ~150 MB) and avoid the force-load dance, but the self-registering algorithm pattern may not work correctly with shared libraries unless the registration code runs at `dlopen` time. Need to test. Static is the safe default.

2. **Formula location** — Personal tap (`madbomber/homebrew-zvec`) vs `homebrew-core`. Core requires significant adoption. Start with a tap.

3. **Upstream install target** — Without one, the formula must manually copy headers and use `libtool` to create the fat archive. With one, it's just `cmake --install build --prefix #{prefix}`.

4. **Linux support** — The formula works on Linux via Linuxbrew. The force-load flag changes to `-Wl,--whole-archive -lzvec -Wl,--no-whole-archive`. The pkg-config file should handle this per-platform.

5. **Version pinning** — The gem should declare a compatible range of zvec library versions. A `zvec-config` header with a version macro would help, or the gem can check at load time.

6. **Fat archive size** — Merging all thirdparty static archives (Arrow alone is ~50 MB) could produce a ~150-200 MB `libzvec.a`. This is large but typical for C++ libraries with vendored dependencies (e.g., `grpc` is similar). The Homebrew bottle (compressed) would be ~40-50 MB.

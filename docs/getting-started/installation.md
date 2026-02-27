# Installation

## Recommended: Homebrew + Gem Install

Pre-build the zvec C++ library with Homebrew, then install the gem:

```bash
brew tap madbomber/zvec https://github.com/MadBomber/zvec-ruby.git
brew install madbomber/zvec/zvec
gem install zvec          # ~10 seconds
```

The gem detects the pre-built library at `/opt/homebrew/opt/zvec` and only compiles the thin Ruby binding layer.

## Alternative: Source Build

Without the Homebrew formula, the gem downloads and compiles the full C++ dependency tree from source:

```bash
gem install zvec          # ~10 minutes
```

This fetches zvec and all its dependencies (Arrow, RocksDB, protobuf, etc.) via CMake's FetchContent during the extension build. It requires CMake, a C++17 compiler, Ninja, and ICU4C.

## Development Setup

For contributing or working on the gem itself:

### Clone with Submodules

```bash
git clone --recurse-submodules https://github.com/MadBomber/zvec-ruby.git
cd zvec-ruby
bundle install
```

### Build the Native Extension

The native extension uses CMake (not the traditional `extconf.rb` approach). Two presets are available:

=== "Release (recommended)"

    ```bash
    cd ext
    cmake --preset macos-release
    cmake --build build/macos-release
    cd ..
    ```

=== "Debug"

    ```bash
    cd ext
    cmake --preset macos-debug
    cmake --build build/macos-debug
    cd ..
    ```

The build outputs `zvec_ext.bundle` directly to the `lib/` directory.

### Verify the Installation

Launch the interactive console:

```bash
bin/console
```

```ruby
irb> Zvec::VERSION
=> "0.0.1"
irb> Zvec::DataType::VECTOR_FP32
=> VECTOR_FP32
```

## Build Presets

The available CMake presets are defined in `ext/CMakePresets.json`:

| Preset | Build Type | C++ Flags | Use Case |
|--------|-----------|-----------|----------|
| `macos-debug` | Debug | `-g -O0` | Development, debugging with lldb |
| `macos-release` | Release | `-O3 -DNDEBUG` | Performance, benchmarking |

Both presets use the Ninja generator and set `CMAKE_PREFIX_PATH` to the Homebrew ICU4C path.

## How the Build Resolves zvec

The gem's CMakeLists.txt uses a three-tier resolution strategy:

1. **Pre-installed library** — Checks for `libzvec.a` at `/opt/homebrew/opt/zvec/lib` (from `brew install zvec`). If found, links against it directly. Fast.
2. **Local submodule** — Uses `vendor/zvec` if present (development checkout with `--recurse-submodules`). Builds from source.
3. **FetchContent** — Downloads zvec from GitHub and builds everything from source. Slowest but works anywhere.

## Troubleshooting

### ICU4C not found

If the build fails with `'unicode/ucnv.h' file not found`:

```bash
brew install icu4c@78
```

The CMake build automatically adds `/opt/homebrew/opt/icu4c@78` to the search path. If your Homebrew prefix is different (e.g., `/usr/local` on Intel Macs), pass it explicitly:

```bash
gem install zvec -- -DCMAKE_PREFIX_PATH=/usr/local/opt/icu4c@78
```

### gem install is slow

Install the Homebrew formula first:

```bash
brew tap madbomber/zvec https://github.com/MadBomber/zvec-ruby.git
brew install madbomber/zvec/zvec
```

Then `gem install zvec` will use the pre-built library and finish in seconds.

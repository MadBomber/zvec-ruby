# Homebrew formula for the zvec C++ vector database library.
#
# Builds zvec from source and installs a single fat static archive (libzvec.a)
# containing all zvec libraries and vendored thirdparty dependencies. The zvec
# Ruby gem links against this pre-built library for fast gem installs.
#
# Usage:
#   brew tap madbomber/zvec https://github.com/MadBomber/zvec-ruby.git
#   brew install madbomber/zvec/zvec
#   gem install zvec            # ~10 seconds instead of ~10 minutes
#
class Zvec < Formula
  desc "Embedded vector database for high-dimensional similarity search"
  homepage "https://github.com/alibaba/zvec"
  url "https://github.com/alibaba/zvec.git",
      tag:      "v0.2.0",
      revision: "1d5aeeed1df6e104443edf23891798f1ff0a8bf7"
  license "Apache-2.0"

  depends_on "cmake" => :build
  depends_on "ninja" => :build
  depends_on "icu4c@78"

  def install
    # Ensure git submodules are initialized (zvec vendors its thirdparty deps)
    system "git", "submodule", "update", "--init", "--recursive"

    # Patch antlr4 for CMake 4.x compatibility — the old CMP00xx policies
    # set to OLD are no longer supported in CMake 4.x
    antlr4_cmake = buildpath/"thirdparty/antlr/antlr4/runtime/Cpp/CMakeLists.txt"
    if antlr4_cmake.exist?
      content = File.read(antlr4_cmake)
      content.gsub!("cmake_minimum_required (VERSION 2.8)", "cmake_minimum_required (VERSION 3.5)")
      content.gsub!(/^\s*CMAKE_POLICY\(SET CMP00\d+ OLD\)\n/, "")
      content.gsub!("endif(NOT CMAKE_BUILD_TYPE)", "endif()")
      content.gsub!("endif(NOT WITH_DEMO)", "endif()")
      content.gsub!("endif(WITH_DEMO)", "endif()")
      content.gsub!("endif(WIN32)", "endif()")
      content.gsub!("if(NOT ANTLR4_CMAKE_DIR)", "if(NOT DEFINED ANTLR4_CMAKE_DIR OR NOT ANTLR4_CMAKE_DIR)")
      content.gsub!("endif(NOT ANTLR4_CMAKE_DIR)", "endif()")
      content.gsub!("endif(ANTLR4_INSTALL)", "endif()")
      File.write(antlr4_cmake, content)
    end

    args = %W[
      -G Ninja
      -DCMAKE_BUILD_TYPE=Release
      -DCMAKE_CXX_STANDARD=17
      -DCMAKE_PREFIX_PATH=#{Formula["icu4c@78"].prefix}
      -DCMAKE_POLICY_VERSION_MINIMUM=3.5
      -DBUILD_PYTHON_BINDINGS=OFF
      -DBUILD_TOOLS=OFF
      -DBUILD_TESTING=OFF
      -DBENCHMARK_ENABLE_TESTING=OFF
      -DBENCHMARK_ENABLE_INSTALL=OFF
      -DPROJECT_ROOT_DIR=#{buildpath}
    ]

    system "cmake", "-S", ".", "-B", "build", *args, *std_cmake_args
    system "cmake", "--build", "build"

    # Install public headers preserving directory structure
    header_src = buildpath/"src/include/zvec"
    (include/"zvec").mkpath
    cp_r header_src.children, include/"zvec/"

    # Create single fat static library from all zvec + thirdparty archives.
    # The algorithm libraries use self-registering factories via static
    # constructors, so consumers must force-load the entire archive.
    #
    # Two dedup challenges:
    #   1. Archives with duplicate member names (e.g. libarrow.a has two
    #      memory.cc.o from different source dirs). `ar x` overwrites the
    #      first with the second, losing symbols. We parse the ar format
    #      directly to extract ALL members with unique filenames.
    #   2. Different projects have .o files with the same basename but
    #      different symbols (e.g. glog's vs Arrow's logging.cc.o), while
    #      Arrow's overlapping archives share identical .o files. We dedup
    #      by MD5 content hash to keep unique .o files and discard true
    #      duplicates.
    require "digest"

    lib.mkpath
    archives = Dir[buildpath/"build/**/*.a"].select { |f| relevant_archive?(f) }

    objdir = buildpath/"_objs"
    objdir.mkpath
    seen_hashes = {}
    unique_objs = []

    archives.each_with_index do |ar_path, i|
      subdir = objdir/"#{i}_#{File.basename(ar_path, ".a")}"
      subdir.mkpath
      extract_ar_members(ar_path, subdir)
      Dir[subdir/"*.o"].each do |obj|
        hash = Digest::MD5.file(obj).hexdigest
        unless seen_hashes[hash]
          seen_hashes[hash] = obj
          unique_objs << obj
        end
      end
    end

    system "libtool", "-static", "-o", lib/"libzvec.a", *unique_objs.sort

    # Install pkg-config file for discovery by the zvec Ruby gem
    (lib/"pkgconfig").mkpath
    (lib/"pkgconfig/zvec.pc").write <<~PC
      prefix=#{prefix}
      libdir=${prefix}/lib
      includedir=${prefix}/include

      Name: zvec
      Description: Embedded vector database for high-dimensional similarity search
      Version: #{version}
      Libs: -L${libdir} -Wl,-force_load,${libdir}/libzvec.a
      Cflags: -I${includedir}
    PC
  end

  test do
    (testpath/"test.cpp").write <<~CPP
      #include <zvec/db/status.h>
      int main() {
        auto s = zvec::Status::OK();
        return s.ok() ? 0 : 1;
      }
    CPP
    system ENV.cxx, "-std=c++17", "test.cpp",
           "-I#{include}", "-L#{lib}",
           "-Wl,-force_load,#{lib}/libzvec.a",
           "-o", "test"
    system "./test"
  end

  private

  def relevant_archive?(path)
    basename = File.basename(path)
    # Exclude test, benchmark, and googletest/googlemock archives
    return false if basename.match?(/test|benchmark|gmock|gtest/i)
    true
  end

  # Extract all .o members from a BSD ar archive, handling duplicate member
  # names by appending a numeric suffix. This avoids the `ar x` problem where
  # later members with the same name silently overwrite earlier ones.
  def extract_ar_members(ar_path, output_dir)
    data = File.binread(ar_path)
    return unless data.start_with?("!<arch>\n")

    pos = 8
    name_count = Hash.new(0)

    while pos + 60 <= data.length
      name_field = data[pos, 16]
      size = data[pos + 48, 10].strip.to_i
      pos += 60

      # BSD extended names: "#1/N" means N bytes of filename prepended to data
      if name_field.start_with?("#1/")
        name_len = name_field[3..].strip.to_i
        name = data[pos, name_len].sub(/\0+\z/, "")
        content_start = pos + name_len
        content_size = size - name_len
      else
        name = name_field.strip.delete_suffix("/")
        content_start = pos
        content_size = size
      end

      if name.end_with?(".o")
        name_count[name] += 1
        unique_name = name_count[name] > 1 ? "#{File.basename(name, ".o")}_dup#{name_count[name]}.o" : name
        File.binwrite("#{output_dir}/#{unique_name}", data[content_start, content_size])
      end

      pos += size
      pos += 1 if pos.odd?
    end
  end
end

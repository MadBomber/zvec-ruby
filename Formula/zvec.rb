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
      revision: "232803611ea91abc96904a92c878c4c67ddf73e6"
  license "Apache-2.0"

  depends_on "cmake" => :build
  depends_on "ninja" => :build
  depends_on "icu4c@78"

  def install
    # Ensure git submodules are initialized (zvec vendors its thirdparty deps)
    system "git", "submodule", "update", "--init", "--recursive"

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
    lib.mkpath
    archives = Dir[buildpath/"build/**/*.a"].select { |f| relevant_archive?(f) }
    system "libtool", "-static", "-o", lib/"libzvec.a", *archives

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
end

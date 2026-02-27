# frozen_string_literal: true

require "bundler/gem_tasks"
require "rake/testtask"

EXT_DIR = File.expand_path("ext", __dir__)
BUNDLE_PATH = File.expand_path("lib/zvec_ext.bundle", __dir__)

def cmake_configure(preset)
  build_dir = File.join(EXT_DIR, "build", preset)
  cache_file = File.join(build_dir, "CMakeCache.txt")

  if File.exist?(cache_file)
    cached_source = File.read(cache_file)[/CMAKE_HOME_DIRECTORY:INTERNAL=(.+)/, 1]
    if cached_source && cached_source != EXT_DIR
      rm_rf build_dir
    end
  end

  sh "cmake --preset #{preset}", chdir: EXT_DIR
end

namespace :ext do
  desc "Configure the C++ extension (debug)"
  task :configure do
    cmake_configure("macos-debug")
  end

  desc "Build the C++ extension (debug)"
  task build: :configure do
    sh "cmake --build build/macos-debug", chdir: EXT_DIR
  end

  desc "Configure the C++ extension (release)"
  task :configure_release do
    cmake_configure("macos-release")
  end

  desc "Build the C++ extension (release)"
  task build_release: :configure_release do
    sh "cmake --build build/macos-release", chdir: EXT_DIR
  end

  desc "Remove build artifacts"
  task :clean do
    rm_rf File.join(EXT_DIR, "build")
    rm_f FileList[File.expand_path("lib/zvec_ext.{bundle,so}", __dir__)]
  end
end

desc "Build the C++ extension (debug)"
task compile: "ext:build"

EXT_SOURCES = FileList["ext/zvec/*.{cpp,hpp}", "ext/CMakeLists.txt"]

desc "Build the C++ extension if sources are newer than the bundle"
task :compile_if_needed do
  if !File.exist?(BUNDLE_PATH) || EXT_SOURCES.any? { |f| File.mtime(f) > File.mtime(BUNDLE_PATH) }
    Rake::Task[:compile].invoke
  end
end

Rake::TestTask.new(:test) do |t|
  t.libs << "test"
  t.libs << "lib"
  t.test_files = FileList["test/**/test_*.rb"]
end

task test: :compile_if_needed
task default: :test

# frozen_string_literal: true

require_relative "lib/zvec/version"

Gem::Specification.new do |spec|
  spec.name    = "zvec"
  spec.version = Zvec::VERSION
  spec.authors = ["Dewayne VanHoozer"]
  spec.email   = ["dewayne@vanhoozer.me"]

  spec.summary     = "EXPERIMENTAL - DO NOT INSTALL - Ruby bindings for Alibaba zvec vector database"
  spec.description = "Highly experimental. Do not install. Native Ruby extension providing bindings to the zvec C++ vector database library via Rice. This gem is an experimental collaboration with Claude and is not ready for production use."
  spec.homepage    = "https://github.com/MadBomber/zvec"
  spec.license     = "MIT"

  spec.required_ruby_version = ">= 3.2.0"

  spec.metadata["homepage_uri"]    = spec.homepage
  spec.metadata["source_code_uri"] = spec.homepage
  spec.metadata["changelog_uri"]   = "#{spec.homepage}/blob/main/CHANGELOG.md"

  spec.files = Dir[
    "lib/**/*.rb",
    "ext/**/*.{cpp,hpp,txt,json}",
    "LICENSE.txt",
    "README.md",
    "CHANGELOG.md"
  ]
  spec.extensions    = ["ext/CMakeLists.txt"]
  spec.require_paths = ["lib"]

  spec.add_dependency "rice", "~> 4.11"

  spec.post_install_message = <<~MSG
    zvec installed successfully!

    Tip: For faster future installs, pre-build the C++ library with Homebrew:
      brew tap madbomber/zvec https://github.com/MadBomber/zvec-ruby.git
      brew install madbomber/zvec/zvec
  MSG
end

# frozen_string_literal: true

require_relative "lib/zvec/version"

Gem::Specification.new do |spec|
  spec.name    = "zvec"
  spec.version = Zvec::VERSION
  spec.authors = ["Dewayne VanHoozer"]
  spec.email   = ["dewayne@vanhoozer.me"]

  spec.summary     = "Fast vector database for Ruby — native bindings to Alibaba's zvec C++ library"
  spec.description = "Add vector search to your Ruby apps without external services. zvec provides native bindings to Alibaba's high-performance C++ vector database via Rice, supporting HNSW, IVF, and flat indexes with multiple distance metrics. Build semantic search, recommendations, RAG pipelines, and similarity matching with pure Ruby — no HTTP APIs, no infrastructure, no latency overhead."
  spec.homepage    = "https://github.com/MadBomber/zvec-ruby"
  spec.license     = "MIT"

  spec.required_ruby_version = ">= 4.0"

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

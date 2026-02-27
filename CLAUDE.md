# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Ruby gem providing native bindings to Alibaba's [zvec](https://github.com/alibaba/zvec) C++ vector database library. The C++ bindings use [Rice](https://github.com/ruby-rice/rice) (v4.11) for Ruby/C++ interop. The zvec C++ source is included as a git submodule at `vendor/zvec`.

## Build Commands

The native extension uses CMake (not extconf.rb):

```bash
# Build the native extension (outputs zvec_ext.bundle to lib/)
cd ext && cmake --preset macos-debug && cmake --build build/macos-debug && cd ..

# Or release build
cd ext && cmake --preset macos-release && cmake --build build/macos-release && cd ..

# Run all tests
bundle exec rake test

# Run a single test file
bundle exec ruby -Ilib:test test/test_collection.rb

# Run a single test method
bundle exec ruby -Ilib:test test/test_collection.rb -n test_query

# Interactive console
bin/console
```

Build prerequisite: `brew install icu4c@78` (CMake expects it at `/opt/homebrew/opt/icu4c@78`).

## Architecture

### Native Extension Layer (`ext/zvec/`)

C++ binding files split by zvec domain, all initialized in dependency order from `zvec_ext.cpp`:

1. **zvec_types** — Enum constants: `DataType`, `IndexType`, `MetricType`, `QuantizeType`, `StatusCode`, `Operator`
2. **zvec_status** — `Status` class and Ruby exception hierarchy (`Zvec::Error` subclasses mapping to `StatusCode` values)
3. **zvec_params** — Index params (`HnswIndexParams`, `FlatIndexParams`, `IVFIndexParams`, `InvertIndexParams`), query params, `CollectionOptions`, `VectorQuery`
4. **zvec_schema** — `FieldSchema` and `CollectionSchema`
5. **zvec_doc** — `Doc` with typed field get/set dispatch
6. **zvec_collection** — `Collection` (create_and_open, open, insert, query, fetch, delete, flush, destroy!)
7. **zvec_config** — Global zvec config

`zvec_common.hpp` provides shared error handling: `throw_if_error()` converts C++ `Status` to Ruby exceptions, `unwrap_result()` unwraps `tl::expected<T, Status>`.

### Ruby Layer (`lib/`)

- **zvec.rb** — Loads `zvec_ext.bundle`, patches `Std::SharedPtr<Collection>` with `CollectionConvenience` via `ObjectSpace` (Rice wraps `shared_ptr<Collection>` as a proxy class that delegates via `method_missing`)
- **collection.rb** — `CollectionConvenience` mixin (adds `query_vector` helper) and `Zvec.open_collection` block form
- **doc.rb** — `Doc#to_h(schema)` for converting docs to Ruby hashes with type dispatch

### Key Design Detail

Rice wraps `shared_ptr<Collection>` as `Std::SharedPtr<zvec::Collection>`, not as `Zvec::Collection`. Ruby convenience methods must be mixed into this wrapper class, which is why `zvec.rb` uses `ObjectSpace.each_object(Class)` to find and patch it at load time.

## Testing

Tests use Minitest. Each test file covers a specific binding module. Collection tests use `Dir.mktmpdir` for isolated on-disk databases and call `col.destroy!` for cleanup.

## Required Ruby Version

>= 3.2.0

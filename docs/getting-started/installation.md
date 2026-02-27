# Installation

## Add the Gem

Add zvec to your `Gemfile`:

```ruby
gem "zvec"
```

Then install dependencies:

```bash
bundle install
```

## Initialize the Git Submodule

The zvec C++ source is included as a git submodule at `vendor/zvec`. After cloning the repository, initialize it:

```bash
git submodule update --init --recursive
```

## Build the Native Extension

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

The build outputs `zvec_ext.bundle` directly to the `lib/` directory, so no additional copy step is needed.

## Verify the Installation

Launch the interactive console to confirm everything loads:

```bash
bin/console
```

```ruby
irb> Zvec::VERSION
=> "0.1.0"
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

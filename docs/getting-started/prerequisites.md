# Prerequisites

Zvec requires a few system dependencies before you can build the native extension.

## Ruby

Ruby **3.2.0** or later is required.

```bash
ruby --version
```

## CMake

CMake **3.26** or later is needed to build the C++ extension.

=== "macOS (Homebrew)"

    ```bash
    brew install cmake ninja
    ```

=== "Ubuntu/Debian"

    ```bash
    sudo apt-get install cmake ninja-build
    ```

## ICU4C

The zvec C++ library depends on ICU for Unicode support (via Apache Arrow).

=== "macOS (Homebrew)"

    ```bash
    brew install icu4c@78
    ```

    The CMake build expects ICU at `/opt/homebrew/opt/icu4c@78`. If your Homebrew prefix differs, adjust the `CMAKE_PREFIX_PATH` in `ext/CMakePresets.json`.

=== "Ubuntu/Debian"

    ```bash
    sudo apt-get install libicu-dev
    ```

## C++ Compiler

A C++17-capable compiler is required:

- **macOS**: Xcode Command Line Tools (`xcode-select --install`)
- **Linux**: GCC 9+ or Clang 10+

## Optional: Pre-built zvec Library (Recommended)

Pre-building the zvec C++ library with Homebrew makes `gem install zvec` fast (~10 seconds instead of ~10 minutes):

```bash
brew tap madbomber/zvec https://github.com/MadBomber/zvec-ruby.git
brew install madbomber/zvec/zvec
```

Without this, the gem will download and compile the full C++ dependency tree (Arrow, RocksDB, protobuf, etc.) from source during installation.

## Verifying Your Environment

Run the following to confirm everything is in place:

```bash
ruby --version      # >= 3.2.0
cmake --version     # >= 3.26
ninja --version     # any recent version
```

If you installed the Homebrew formula:

```bash
brew info zvec      # shows installed version and prefix
pkg-config --libs zvec  # should print -Wl,-force_load,...
```

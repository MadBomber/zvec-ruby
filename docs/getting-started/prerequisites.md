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

The zvec C++ library depends on ICU for Unicode support.

=== "macOS (Homebrew)"

    ```bash
    brew install icu4c@78
    ```

    The CMake preset expects ICU at `/opt/homebrew/opt/icu4c@78`. If your Homebrew prefix differs, adjust the `CMAKE_PREFIX_PATH` in `ext/CMakePresets.json`.

=== "Ubuntu/Debian"

    ```bash
    sudo apt-get install libicu-dev
    ```

## C++ Compiler

A C++17-capable compiler is required:

- **macOS**: Xcode Command Line Tools (`xcode-select --install`)
- **Linux**: GCC 9+ or Clang 10+

## Verifying Your Environment

Run the following to confirm everything is in place:

```bash
ruby --version      # >= 3.2.0
cmake --version     # >= 3.26
ninja --version     # any recent version
```

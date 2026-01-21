# Building Wren with CMake

Wren uses CMake as its build system for cross-platform compilation.

## Quick Start

```bash
# Configure the build
cmake -B build

# Build the project
cmake --build build

# Run tests
./build/bin/wren_test
```

## Build Targets

The CMake configuration generates the following targets:

- **wren** - Static library (`lib/libwren.a`)
- **wren_shared** - Shared/dynamic library (`lib/libwren.so`)
- **wren_test** - Test executable (`bin/wren_test`)

## Build Options

You can customize the build with these CMake options:

### BUILD_TESTS
Build the test executable (default: ON)

```bash
cmake -B build -DBUILD_TESTS=OFF
```

### WREN_NAN_TAGGING
Enable NaN tagging optimization for performance (default: ON)

```bash
# Disable NaN tagging
cmake -B build -DWREN_NAN_TAGGING=OFF
```

### CMAKE_BUILD_TYPE
Set the build configuration (Release or Debug)

```bash
# Release build (optimized)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Debug build (with symbols)
cmake -B build -DCMAKE_BUILD_TYPE=Debug
```

## Platform-Specific Builds

### Linux/macOS
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

### Windows (Visual Studio)
```bash
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

### Windows (MinGW)
```bash
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

## Installation

Install Wren to your system:

```bash
# Configure with install prefix
cmake -B build -DCMAKE_INSTALL_PREFIX=/usr/local

# Build
cmake --build build

# Install (may require sudo)
sudo cmake --install build
```

This installs:
- Headers to `<prefix>/include/`
- Libraries to `<prefix>/lib/`
- CMake config files to `<prefix>/lib/cmake/Wren/`

## Using Wren in Your CMake Project

After installation, you can use Wren in your CMake project:

```cmake
find_package(Wren REQUIRED)

add_executable(my_app main.c)
target_link_libraries(my_app PRIVATE Wren::wren)
```

## Advanced Options

### 32-bit Build
```bash
cmake -B build -DCMAKE_C_FLAGS="-m32"
```

### Custom Compiler
```bash
cmake -B build -DCMAKE_C_COMPILER=clang
```

### Parallel Build
```bash
cmake --build build -j8  # Use 8 cores
```

### Clean Build
```bash
rm -rf build
cmake -B build
cmake --build build
```

## Migrating from Old Build System

The old Premake5/Make-based build system has been replaced with CMake. Key differences:

| Old System | CMake Equivalent |
|------------|------------------|
| `premake5 gmake2 && make` | `cmake -B build && cmake --build build` |
| `projects/make/` | `build/` |
| `lib/` output | `build/lib/` |
| `bin/` output | `build/bin/` |

## Troubleshooting

### "CMake not found"
Install CMake 3.15 or later from your package manager or [cmake.org](https://cmake.org/download/)

### Build failures
Clean the build directory and try again:
```bash
rm -rf build
cmake -B build
cmake --build build
```

### Missing math library (Unix)
This is handled automatically by CMake on Unix-like systems.

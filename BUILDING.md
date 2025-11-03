# Building Xypher

Quick build instructions for all platforms.

## Prerequisites

See [INSTALLATION.md](INSTALLATION.md) for complete installation instructions.

Required:
- CMake 3.20+
- LLVM 14+ (LLVM 21 recommended)
- C++20 compiler (Clang/GCC/MSVC)
- Clang for linking

## Quick Build

```bash
./build.sh         # Linux/macOS
.\build.bat        # Windows
```

Output: `build/bin/`

## Platform-Specific

### Windows

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### Linux

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### macOS

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)
```

## CMake Options

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release           # Release build
cmake .. -DCMAKE_BUILD_TYPE=Debug             # Debug build
cmake .. -DLLVM_DIR=/path/to/llvm            # Custom LLVM
cmake .. -DLLVM_ENABLE_DIA_SDK=OFF           # Disable DIA SDK
```

## Verification

```bash
./build/bin/xypc --version
./build/bin/xypc examples/hello.xyp -o hello
./hello
```

## Troubleshooting

**LLVM not found:**
```bash
llvm-config --prefix
export LLVM_DIR=$(llvm-config --prefix)/lib/cmake/llvm
```

**Linking errors:**
Make sure Clang is installed: `which clang`

**Build stuck:**
Clean and rebuild: `rm -rf build && ./build.sh`

## Development Build

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build . --verbose
```

This generates `compile_commands.json` for IDE integration.

## Next Steps

After successful build:
1. Test: `./build/bin/xypc examples/hello.xyp -o hello`
2. Run: `./hello`
3. Try all examples: `./test_all_examples.bat`

See [README.md](README.md) for usage documentation.

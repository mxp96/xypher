# Xypher Programming Language

A compiled language built on LLVM.

## Features

- Static typing with type inference
- LLVM-based code generation
- Cross-platform (Windows, Linux, macOS)
- Standard library (I/O, math, arrays, files, hashmap)
- Multiple optimization levels (O0-O3, Os, Oz)

## Getting Started

See [QUICKSTART.md](QUICKSTART.md) for your first program and examples.

## Installation

See [INSTALLATION.md](INSTALLATION.md) for complete installation instructions for all platforms.

## Building

See [BUILDING.md](BUILDING.md) for build instructions.

Quick build:
```bash
./build.sh    # Linux/macOS
.\build.bat   # Windows
```

## Usage

```bash
xypc program.xyp -o program
./program
```

### Options

- `-o <file>` - Output name
- `--emit-llvm` - Generate LLVM IR
- `--check-syntax` - Syntax check only
- `--ast-dump` - Show AST
- `-O<0-3>` - Optimization level
- `-Os` - Size optimization
- `-h` - Help

## Documentation

- [Installation](INSTALLATION.md) - Dependencies and setup
- [Building](BUILDING.md) - Build instructions
- [Quickstart](QUICKSTART.md) - Getting started guide
- [Language Spec](LANGUAGE_SPEC.md) - Syntax reference
- [Standard Library](STDLIB.md) - API documentation

## License

Apache 2.0 License

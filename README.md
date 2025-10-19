# Xypher Programming Language

A compiled language featuring type safety and manual memory control. Built on LLVM for native performance.

## Features

- Type-safe with explicit ownership (`own` keyword)
- LLVM-powered native compilation
- Cross-platform (Windows, Linux, macOS)
- Unique syntax with `say()`, `trace()`, `loopwhile()`
- Optimization levels O0-O3
- Standard library with 25+ functions (I/O, math, arrays, files)

## Quick Example

```xypher
func factorial(n: i32) -> i32 {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

func main() -> i32 {
    let result = factorial(5);
    say("Factorial:", result);
    return 0;
}
```

## Unique Keywords

- `say()` - Print to console
- `trace()` - Debug with type info
- `own` - Explicit ownership
- `loopwhile()` - Loop construct
- `grab()` - User input (planned)
- `link` - Module import (planned)
- `fall()` - Fallback return (planned)

## Installation

### Requirements

- LLVM 14+
- CMake 3.20+
- C++20 compiler (Clang/GCC/MSVC)
- Clang for linking

#### Quick Install

**Windows:**
```bash
choco install llvm cmake
```

**Linux:**
```bash
sudo apt install llvm-14-dev clang cmake build-essential
```

**macOS:**
```bash
brew install llvm cmake
```

### Build

```bash
git clone <repo-url>
cd xypher-lang
./build.bat        # Windows
./build.sh         # Linux/macOS
```

Output: `build/bin/xypc` and `build/bin/xystd.dll`

## Usage

### Basic Compilation

```bash
xypc program.xyp -o program
```

### Options

- `-o <file>` - Output name
- `--emit-llvm` - Generate LLVM IR
- `--check-syntax` - Syntax check only
- `--ast-dump` - Show AST
- `-O<0-3>` - Optimization level
- `-h` - Help

### Examples

```bash
build\bin\xypc.exe examples\hello.xyp -o output\hello
output\hello.exe

# With optimization
build\bin\xypc.exe program.xyp -O3 -o program

# Test all examples
.\build.bat test
```

## Language Syntax

### Variables

```xypher
let x: i32 = 42;           // Mutable
const PI: f64 = 3.14159;   // Immutable
own data: i32 = 100;       // Owned
```

### Types

Integers: `i8` `i16` `i32` `i64` `u8` `u16` `u32` `u64`  
Floats: `f32` `f64`  
Others: `bool` `char` `str` `void`

### Functions

```xypher
func add(a: i32, b: i32) -> i32 {
    return a + b;
}

func greet(name: str) -> void {
    say("Hello,", name);
}
```

### Control Flow

```xypher
if (x > 0) {
    say("positive");
}

loopwhile (i < 10) {
    say(i);
    i = i + 1;
}
```

### Debugging

```xypher
trace(value);  // Outputs: [TRACE] value: i32 = 42
```

## Architecture

```
.xyp -> Lexer -> Parser -> AST -> Semantic Analysis -> 
LLVM IR -> Optimizer -> Object -> Binary
```

Components:
- **Frontend**: Lexer, Parser, Type checker
- **Backend**: LLVM code generation
- **Runtime**: Standard library (xystd)


## Examples

Basic:
- `hello.xyp` - Hello World
- `variables.xyp` - Variable types
- `functions.xyp` - Functions
- `control_flow.xyp` - Loops and conditionals

Intermediate:
- `fibonacci.xyp` - Fibonacci sequence
- `recursion.xyp` - Recursive algorithms
- `advanced_math.xyp` - GCD, LCM, primes
- `arrays.xyp` - Array operations
- `string_ops.xyp` - String manipulation

Advanced:
- `structs.xyp` - Struct simulation
- `enums.xyp` - Enum patterns
- `pattern_matching.xyp` - Pattern matching simulation
- `modules.xyp` - Module system demo
- `math_operations.xyp` - Math operators
- `trace_debug.xyp` - Debug tracing

## Roadmap

**v1.0** (Current): Compiler, LLVM backend, stdlib (49 functions), JIT foundation  
**v1.1** (Next): Full modules, structs, enums, pattern matching syntax  
**v2.0** (Future): Async/await, generics, LSP, package manager

## Contributing

Fork, create branch, commit, push, PR. See [CONTRIBUTING.md](CONTRIBUTING.md).

## License

Apache 2.0 License

## Troubleshooting

**LLVM not found?**
```bash
export LLVM_DIR=/path/to/llvm
```

**Build fails?**
```bash
rm -rf build && mkdir build && cd build && cmake ..
```

**Library not found at runtime?**
Copy `xystd.dll` to same folder as your program.


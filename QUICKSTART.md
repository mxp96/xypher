# Xypher Quick Start

Get started with Xypher in 5 minutes.

## Install

**Windows:**
```bash
choco install llvm cmake
git clone <repo-url> && cd xypher-lang
.\build.bat
```

**Linux:**
```bash
sudo apt install llvm-14-dev clang cmake build-essential
git clone <repo-url> && cd xypher-lang
./build.sh
```

**macOS:**
```bash
brew install llvm cmake
git clone <repo-url> && cd xypher-lang
./build.sh
```

## First Program

Create `hello.xyp`:
```xypher
func main() -> i32 {
    say("Hello from Xypher!");
    return 0;
}
```

Compile and run:
```bash
build/bin/xypc hello.xyp -o hello
./hello
```

## Basic Examples

### Variables

```xypher
func main() -> i32 {
    let x: i32 = 42;           // Mutable integer
    const PI: f64 = 3.14159;   // Immutable float
    own data: i32 = 100;       // Owned variable
    
    say("x =", x);
    say("PI =", PI);
    
    return 0;
}
```

### Functions

```xypher
func add(a: i32, b: i32) -> i32 {
    return a + b;
}

func greet(name: str) -> void {
    say("Hello,", name);
}

func main() -> i32 {
    let sum = add(10, 20);
    say("Sum:", sum);
    
    greet("Developer");
    
    return 0;
}
```

### Control Flow

```xypher
func checkNumber(n: i32) -> void {
    if (n > 0) {
        say("Positive");
    } else {
        say("Non-positive");
    }
}

func countDown(n: i32) -> void {
    loopwhile (n > 0) {
        say(n);
        n = n - 1;
    }
}

func main() -> i32 {
    checkNumber(5);
    countDown(3);
    return 0;
}
```

### Debug Tracing

```xypher
func calculate(x: i32) -> i32 {
    trace(x);              // Print debug info
    let result = x * 2;
    trace(result);
    return result;
}

func main() -> i32 {
    let value = calculate(21);
    say("Result:", value);
    return 0;
}
```

## Compiler Options

```bash
xypc file.xyp -o output      # Compile
xypc file.xyp -O3 -o out     # Optimize
xypc file.xyp --emit-llvm    # LLVM IR
xypc file.xyp --check-syntax # Syntax check
xypc file.xyp --ast-dump     # Show AST
```

## Types

Integers: `i8` `i16` `i32` `i64` `u8` `u16` `u32` `u64`  
Floats: `f32` `f64`  
Others: `bool` `char` `str` `void`

## Operators

**Arithmetic**: `+` `-` `*` `/` `%`  
**Comparison**: `==` `!=` `<` `<=` `>` `>=`  
**Logical**: `&&` `||` `!`  
**Bitwise**: `&` `|` `^` `~` `<<` `>>`  
**Assignment**: `=` `+=` `-=` `*=` `/=`

## Unique Features

**`say()`** - Print output
```xypher
say("Hello");
say("x =", x);
```

**`trace()`** - Debug with type info
```xypher
trace(value);  // [TRACE] value: i32 = 42
```

**`own`** - Explicit ownership
```xypher
own data: i32 = 100;
```

**`loopwhile()`** - Loop construct
```xypher
loopwhile (i < 10) {
    i = i + 1;
}
```

## Program Structure

Every Xypher program starts with `main`:
```xypher
func main() -> i32 {
    // your code
    return 0;
}
```

## Complete Example

```xypher
func fibonacci(n: i32) -> i32 {
    if (n <= 1) {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

func main() -> i32 {
    let i: i32 = 0;
    
    loopwhile (i < 10) {
        let fib = fibonacci(i);
        say("F(", i, ") =", fib);
        i = i + 1;
    }
    
    return 0;
}
```

## More Examples

See `examples/` for more programs:
- `arrays.xyp` - Array operations
- `structs.xyp` - Struct patterns
- `enums.xyp` - Enum constants
- `pattern_matching.xyp` - Conditional patterns
- `recursion.xyp` - Recursive algorithms
- `advanced_math.xyp` - Math utilities
- `string_ops.xyp` - String handling
- `modules.xyp` - Module system demo

## Troubleshooting

**Build fails?** `.\build.bat clean && .\build.bat`  
**LLVM not found?** Set `LLVM_DIR` environment variable  
**Runtime error?** Ensure `xystd.dll` is in same folder

## Next Steps

- Try all examples: `.\build.bat test`
- Read [LANGUAGE_SPEC.md](LANGUAGE_SPEC.md)
- Check [STDLIB.md](STDLIB.md) for API

## Docs

- [README.md](README.md) - Main documentation
- [LANGUAGE_SPEC.md](LANGUAGE_SPEC.md) - Language reference
- [STDLIB.md](STDLIB.md) - Standard library API
- [CONTRIBUTING.md](CONTRIBUTING.md) - How to contribute


# Xypher Language Specification

Version 1.0.0

## Introduction

Xypher is a simple compiled language with static typing.

## Lexical Structure

### Comments

```xypher
// Single-line comment
```

### Identifiers

Start with letter or underscore: `myVar`, `_private`, `PascalCase`

### Keywords

Reserved words:
```
func return if else loopwhile while for break continue
let const own type struct enum impl trait
say grab link fall trace
match in as is
async await spawn
true false null
```

### Literals

```xypher
42          // Integer
3.14        // Float
"string"    // String
'c'         // Char
true/false  // Boolean
```

## Types

### Primitives

**Integers**: `i8` `i16` `i32` `i64` `u8` `u16` `u32` `u64`  
**Floats**: `f32` `f64`  
**Others**: `bool` `char` `str` `void`

### Type Inference

```xypher
let x = 42;        // Inferred as i32
let y = 3.14;      // Inferred as f64
```

### Explicit Types

```xypher
let x: i32 = 42;
let y: f64 = 3.14;
```

## Expressions

### Literals

```xypher
42
3.14
"hello"
'a'
true
```

### Binary

```xypher
a + b    // Arithmetic
x == y   // Comparison
a && b   // Logical
```

### Unary

```xypher
-value
!flag
~bits
```

### Calls

```xypher
functionName(arg1, arg2)
```

## Statements

### Variables

```xypher
let x: i32 = 42;      // Mutable
const PI: f64 = 3.14; // Immutable
own data: i32 = 100;  // Owned
```

### Blocks

```xypher
{
    let x = 10;
    say(x);
}
```

### If

```xypher
if (condition) {
    // then
} else {
    // else
}
```

### Loops

```xypher
loopwhile (i < 10) {
    i = i + 1;
}

while (condition) {
    // body
}

for (let i = 0; i < 10; i = i + 1) {
    // body
}
```

### Return

```xypher
return;
return value;
```

### Say (Output)

```xypher
say("Hello");
say("x =", x);
```

### Trace (Debug)

```xypher
trace(variable);
```

## Functions

```xypher
func name(param: Type) -> ReturnType {
    return value;
}
```

Void function:
```xypher
func greet(name: str) -> void {
    say("Hello,", name);
}
```

Recursive:
```xypher
func factorial(n: i32) -> i32 {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}
```

## Operators

### Precedence (high to low)

1. Call: `()`
2. Unary: `-` `!` `~`
3. Multiplicative: `*` `/` `%`
4. Additive: `+` `-`
5. Shift: `<<` `>>`
6. Bitwise: `&` `^` `|`
7. Comparison: `<` `<=` `>` `>=`
8. Equality: `==` `!=`
9. Logical: `&&` `||`
10. Assignment: `=` `+=` `-=`

### Arithmetic

`+` `-` `*` `/` `%`

### Comparison

`==` `!=` `<` `<=` `>` `>=`

### Logical

`&&` `||` `!`

### Bitwise

`&` `|` `^` `~` `<<` `>>`

### Assignment

`=` `+=` `-=` `*=` `/=`

## Ownership

```xypher
own x: i32 = 42;  // Explicitly owned
```

Ownership transfer on assignment (move semantics).

## Planned Features

### v1.1
- Modules (`link`)
- Structs and enums
- Pattern matching (`match`)

### v2.0
- Async/await
- Generics
- Traits

## Examples

See [QUICKSTART.md](QUICKSTART.md) for examples and tutorials.

See `examples/` directory for complete programs.

## Grammar

Simplified EBNF:

```ebnf
program    = declaration* EOF
declaration = funcDecl | varDecl | statement
funcDecl   = "func" IDENT "(" params ")" "->" type block
varDecl    = ("let" | "const" | "own") IDENT ":" type "=" expr ";"
statement  = exprStmt | block | if | loop | return | say | trace
expression = assignment | logical | comparison | arithmetic | unary | primary
```

Full grammar available in parser source code.

## See Also

- [QUICKSTART.md](QUICKSTART.md) - Getting started guide
- [BUILDING.md](BUILDING.md) - Build instructions
- [STDLIB.md](STDLIB.md) - Standard library API
- [README.md](README.md) - Project overview

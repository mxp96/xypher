# Changelog

## [1.0.0] - 2025-10-19

### Added

**Compiler**:
- Complete lexer and parser
- Type checker and semantic analyzer
- LLVM code generation backend
- Optimization support (O0-O3)
- CLI with multiple modes

**Language Features**:
- Type system: i8-i64, u8-u64, f32-f64, bool, char, str
- Functions with parameters and return types
- Control flow: if/else, loopwhile, for, while
- Operators: arithmetic, comparison, logical, bitwise
- Type inference
- Explicit ownership (`own` keyword)

**Unique Syntax**:
- `say()` - Console output
- `trace()` - Debug with type info
- `loopwhile()` - Loop construct
- `own` - Ownership declaration

**Standard Library**:
- Shared library (xystd.dll/.so/.dylib)
- I/O functions
- Debug tracing
- Memory management
- String operations
- Math functions
- Runtime support

**Tooling**:
- Build scripts for all platforms
- Example programs (7 samples)
- Auto-detection of standard library
- Output directory creation

**Examples** (13 programs):
- Basic: hello, variables, functions, control_flow
- Intermediate: fibonacci, recursion, advanced_math, arrays, string_ops
- Advanced: structs, enums, pattern_matching, modules
- Debug: math_operations, trace_debug

**Documentation**:
- README with quick start
- Language specification
- Building guide
- Standard library docs (25+ functions)
- Deployment guide

### Known Issues

- Module system not implemented
- Structs/enums not implemented
- Pattern matching not implemented
- Some stdlib functions are stubs

## [Unreleased]

### Planned for v1.1

- Module system (`link` directive)
- Struct and enum types
- Pattern matching (`match`)
- Enhanced error messages
- More stdlib functions

### Planned for v2.0

- Async/await
- Generics
- Trait system
- Package manager
- LSP support
- JIT compilation

---

See [README.md](README.md) for current features.

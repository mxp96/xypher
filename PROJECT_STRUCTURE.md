# Project Structure

Overview of Xypher codebase organization.

## Components

### Frontend
- **Lexer**: Tokenization
- **Parser**: Builds AST
- **Semantic**: Type checking

### Backend
- **CodeGen**: LLVM IR generation
- **Optimizer**: LLVM passes
- **Target**: Platform-specific codegen

### Standard Library
Separate shared library with I/O, debug, memory, string, and math functions.

## Compilation Pipeline

```
.xyp file
  -> Lexer
Tokens
  -> Parser
AST
  -> Semantic
Typed AST
  -> CodeGen
LLVM IR
  -> Optimizer
Optimized IR
  -> Backend
Object file
  -> Linker
Executable (+ xystd)
```

## Key Files

- `src/main.cpp` - Compiler driver
- `src/lexer/Lexer.cpp` - Tokenizer
- `src/parser/Parser.cpp` - Parser
- `src/sema/SemanticAnalyzer.cpp` - Type checker
- `src/codegen/CodeGenerator.cpp` - LLVM codegen
- `std/include/xystd.h` - Standard library API

## Design Patterns

- **Visitor Pattern**: AST traversal
- **Builder Pattern**: Token/AST construction
- **Factory Pattern**: LLVM type creation

## Adding Features

**New keyword**: Add to `TokenType` in `Token.h`  
**New AST node**: Add to `AST.h` and implement visitor  
**New semantic check**: Extend `SemanticAnalyzer`  
**New stdlib function**: Add to `std/src/` and `xystd.h`

## Testing

Run all examples:
```bash
./test_all_examples.bat
```

Individual test:
```bash
build/bin/xypc examples/hello.xyp --check-syntax
```

See main [README.md](README.md) for more details.

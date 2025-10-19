# Contributing to Xypher

Thanks for your interest! Here's how to contribute.

## Quick Start

1. Fork the repo
2. Create branch: `git checkout -b feature/my-feature`
3. Make changes
4. Test: `./test_all_examples.bat`
5. Commit: `git commit -m "Add feature X"`
6. Push and create PR

## Code Style

- C++20 standard
- Classes: `PascalCase`
- Functions: `camelCase`
- Variables: `camelCase`
- Private members: `trailing_`
- Meaningful names, not abbreviations

Example:
```cpp
class CodeGenerator {
public:
    void generateCode();
private:
    Lexer& lexer_;
    Token currentToken_;
};
```

## Commit Messages

Format: `type(scope): description`

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation
- `refactor`: Code restructure
- `test`: Add tests

Examples:
```
feat(parser): add match expression support
fix(codegen): resolve void function call issue
docs(readme): update installation steps
```

## Testing

Test your changes:
```bash
cd build
cmake --build . --config Release
../test_all_examples.bat
```

Add tests for new features.

## Documentation

Update relevant docs:
- `README.md` - If adding major feature
- `LANGUAGE_SPEC.md` - If changing syntax
- `STDLIB.md` - If adding stdlib function
- Code comments for complex logic

## PR Guidelines

- One feature per PR
- Include tests
- Update documentation
- Pass all existing tests
- Clear description of changes

## Areas to Contribute

**Easy**:
- Add example programs
- Fix typos
- Improve error messages
- Add tests

**Medium**:
- Implement structs/enums
- Add stdlib functions
- Improve optimizations

**Hard**:
- Module system
- Pattern matching
- LSP support
- Generics

## Questions?

- Check existing issues
- Ask in discussions
- Read documentation

## Code of Conduct

Be respectful, constructive, and professional.

## License

By contributing, you agree to Apache 2.0 License.

Thanks for making Xypher better!

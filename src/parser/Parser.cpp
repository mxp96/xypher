#include "parser/Parser.h"

namespace xypher {

Parser::Parser(Lexer& lexer, DiagnosticEngine& diags)
    : lexer_(lexer), diags_(diags), current_(Token(TokenType::Unknown, "", SourceLocation())),
      previous_(Token(TokenType::Unknown, "", SourceLocation())) {
    advance();
}

void Parser::advance() {
    previous_ = current_;
    current_ = lexer_.nextToken();
    
    // Debug: print token (uncomment for debugging)
    // std::cerr << "DEBUG Token: " << current_.toString() << "\n";
    
    if (current_.is(TokenType::Unknown)) {
        errorAtCurrent("Unknown token");
    }
}

bool Parser::check(TokenType type) const {
    return current_.is(type);
}

bool Parser::match(TokenType type) {
    if (!check(type)) return false;
    advance();
    return true;
}

bool Parser::consume(TokenType type, const String& message) {
    if (check(type)) {
        advance();
        return true;
    }
    
    errorAtCurrent(message);
    return false;
}

void Parser::error(const String& message) {
    diags_.error(message, current_.getLocation());
}

void Parser::errorAtCurrent(const String& message) {
    diags_.error(message, current_.getLocation());
}

void Parser::errorAtPrevious(const String& message) {
    diags_.error(message, previous_.getLocation());
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous_.is(TokenType::Semicolon)) return;
        
        switch (current_.getType()) {
            case TokenType::KwFunc:
            case TokenType::KwImport:
            case TokenType::KwLet:
            case TokenType::KwConst:
            case TokenType::KwIf:
            case TokenType::KwWhile:
            case TokenType::KwFor:
            case TokenType::KwReturn:
                return;
            default:
                break;
        }
        
        advance();
    }
}

Unique<Program> Parser::parseProgram() {
    Vec<Unique<Stmt>> decls;
    
    while (!isAtEnd()) {
        // Stop if too many errors
        if (diags_.getErrorCount() > 10) {
            error("Too many errors, stopping parse");
            break;
        }
        
        auto decl = declaration();
        if (decl) {
            decls.push_back(std::move(decl));
        } else {
            // Synchronize to avoid infinite loop on error
            synchronize();
        }
    }
    
    return makeUnique<Program>(std::move(decls), SourceLocation());
}

Unique<Stmt> Parser::declaration() {
    if (match(TokenType::KwImport)) {
        return importDecl();
    }
    if (match(TokenType::KwFunc)) {
        return funcDecl();
    }
    if (match(TokenType::KwLet, TokenType::KwConst, TokenType::KwOwn)) {
        return varDecl();
    }
    
    return statement();
}

Unique<Stmt> Parser::funcDecl() {
    auto loc = previous_.getLocation();
    
    if (!consume(TokenType::Identifier, "Expected function name")) {
        return nullptr;
    }
    String name = previous_.getLexeme();
    
    if (!consume(TokenType::LeftParen, "Expected '(' after function name")) {
        return nullptr;
    }
    auto params = parseParameters();
    if (!consume(TokenType::RightParen, "Expected ')' after parameters")) {
        return nullptr;
    }
    
    Unique<TypeNode> returnType;
    if (match(TokenType::Arrow)) {
        returnType = parseType();
    } else {
        returnType = makeUnique<TypeName>("void", loc);
    }
    
    if (!consume(TokenType::LeftBrace, "Expected '{' before function body")) {
        return nullptr;
    }
    
    auto body = dynamic_cast<BlockStmt*>(blockStatement().release());
    
    return makeUnique<FuncDecl>(
        name, std::move(params), std::move(returnType),
        Unique<BlockStmt>(body), loc
    );
}

Unique<Stmt> Parser::importDecl() {
    auto loc = previous_.getLocation();
    
    if (!consume(TokenType::Identifier, "Expected module name after 'import'")) {
        return nullptr;
    }
    String module = previous_.getLexeme();
    
    if (!consume(TokenType::KwFrom, "Expected 'from' after module name")) {
        return nullptr;
    }
    
    if (!consume(TokenType::Identifier, "Expected source library name after 'from'")) {
        return nullptr;
    }
    String source = previous_.getLexeme();
    
    if (!consume(TokenType::Semicolon, "Expected ';' after import statement")) {
        return nullptr;
    }
    
    return makeUnique<ImportDecl>(module, source, loc);
}

Unique<Stmt> Parser::varDecl() {
    auto loc = previous_.getLocation();
    bool isConst = previous_.is(TokenType::KwConst);
    bool isOwned = previous_.is(TokenType::KwOwn);
    
    if (!consume(TokenType::Identifier, "Expected variable name")) {
        return nullptr;
    }
    String name = previous_.getLexeme();
    
    Unique<TypeNode> type;
    if (match(TokenType::Colon)) {
        type = parseType();
    }
    
    Unique<Expr> init;
    if (match(TokenType::Equal)) {
        init = expression();
    }
    
    if (!consume(TokenType::Semicolon, "Expected ';' after variable declaration")) {
        return nullptr;
    }
    
    return makeUnique<VarDecl>(name, std::move(type), std::move(init),
                               isOwned, isConst, loc);
}

Unique<Stmt> Parser::statement() {
    if (match(TokenType::KwIf)) return ifStatement();
    if (match(TokenType::KwReturn)) return returnStatement();
    if (match(TokenType::KwLoopwhile)) return loopwhileStatement();
    if (match(TokenType::KwWhile)) return whileStatement();
    if (match(TokenType::KwFor)) return forStatement();
    if (match(TokenType::KwSay)) return sayStatement();
    if (match(TokenType::KwTrace)) return traceStatement();
    if (match(TokenType::LeftBrace)) return blockStatement();
    
    return exprStatement();
}

Unique<Stmt> Parser::exprStatement() {
    auto loc = current_.getLocation();
    auto expr = expression();
    if (!expr) {
        return nullptr;
    }
    if (!consume(TokenType::Semicolon, "Expected ';' after expression")) {
        return nullptr;
    }
    return makeUnique<ExprStmt>(std::move(expr), loc);
}

Unique<Stmt> Parser::blockStatement() {
    auto loc = previous_.getLocation();
    Vec<Unique<Stmt>> stmts;
    
    Token lastToken = current_;
    int errorCount = 0;
    
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        // Safety: if token didn't advance and we had error, break to avoid infinite loop
        if (current_.getLexeme() == lastToken.getLexeme() && 
            current_.getLocation().getLine() == lastToken.getLocation().getLine() &&
            current_.getLocation().getColumn() == lastToken.getLocation().getColumn()) {
            errorCount++;
            if (errorCount > 3) {
                error("Parser stuck, skipping to next token");
                advance(); // Force advance
                errorCount = 0;
            }
        } else {
            errorCount = 0;
        }
        
        lastToken = current_;
        
        // Check if we have too many errors, stop parsing
        if (diags_.getErrorCount() > 10) {
            error("Too many errors, stopping parse");
            break;
        }
        
        auto stmt = declaration();
        if (stmt) {
            stmts.push_back(std::move(stmt));
        } else {
            synchronize();
        }
    }
    
    consume(TokenType::RightBrace, "Expected '}' after block");
    return makeUnique<BlockStmt>(std::move(stmts), loc);
}

Unique<Stmt> Parser::ifStatement() {
    auto loc = previous_.getLocation();
    
    consume(TokenType::LeftParen, "Expected '(' after 'if'");
    auto cond = expression();
    consume(TokenType::RightParen, "Expected ')' after condition");
    
    auto thenBranch = statement();
    
    Unique<Stmt> elseBranch;
    if (match(TokenType::KwElse)) {
        elseBranch = statement();
    }
    
    return makeUnique<IfStmt>(std::move(cond), std::move(thenBranch),
                              std::move(elseBranch), loc);
}

Unique<Stmt> Parser::returnStatement() {
    auto loc = previous_.getLocation();
    
    Unique<Expr> value;
    if (!check(TokenType::Semicolon)) {
        value = expression();
    }
    
    consume(TokenType::Semicolon, "Expected ';' after return statement");
    return makeUnique<ReturnStmt>(std::move(value), loc);
}

Unique<Stmt> Parser::loopwhileStatement() {
    auto loc = previous_.getLocation();
    
    consume(TokenType::LeftParen, "Expected '(' after 'loopwhile'");
    auto cond = expression();
    consume(TokenType::RightParen, "Expected ')' after condition");
    
    auto body = statement();
    
    return makeUnique<LoopwhileStmt>(std::move(cond), std::move(body), loc);
}

Unique<Stmt> Parser::whileStatement() {
    auto loc = previous_.getLocation();
    
    consume(TokenType::LeftParen, "Expected '(' after 'while'");
    auto cond = expression();
    consume(TokenType::RightParen, "Expected ')' after condition");
    
    auto body = statement();
    
    return makeUnique<LoopwhileStmt>(std::move(cond), std::move(body), loc);
}

Unique<Stmt> Parser::forStatement() {
    auto loc = previous_.getLocation();
    
    consume(TokenType::LeftParen, "Expected '(' after 'for'");
    
    // Simplified for now - would need more complex handling
    auto init = declaration();
    auto cond = expression();
    consume(TokenType::Semicolon, "Expected ';' after condition");
    auto increment = expression();
    consume(TokenType::RightParen, "Expected ')' after for clauses");
    
    auto body = statement();
    
    // Transform for into while loop
    Vec<Unique<Stmt>> stmts;
    stmts.push_back(std::move(init));
    
    Vec<Unique<Stmt>> loopBody;
    loopBody.push_back(std::move(body));
    loopBody.push_back(makeUnique<ExprStmt>(std::move(increment), loc));
    
    auto whileBody = makeUnique<BlockStmt>(std::move(loopBody), loc);
    auto whileLoop = makeUnique<LoopwhileStmt>(std::move(cond), std::move(whileBody), loc);
    
    stmts.push_back(std::move(whileLoop));
    
    return makeUnique<BlockStmt>(std::move(stmts), loc);
}

Unique<Stmt> Parser::sayStatement() {
    auto loc = previous_.getLocation();
    
    consume(TokenType::LeftParen, "Expected '(' after 'say'");
    
    Vec<Unique<Expr>> exprs;
    if (!check(TokenType::RightParen)) {
        do {
            exprs.push_back(expression());
        } while (match(TokenType::Comma));
    }
    
    consume(TokenType::RightParen, "Expected ')' after arguments");
    consume(TokenType::Semicolon, "Expected ';' after say statement");
    
    return makeUnique<SayStmt>(std::move(exprs), loc);
}

Unique<Stmt> Parser::traceStatement() {
    auto loc = previous_.getLocation();
    
    consume(TokenType::LeftParen, "Expected '(' after 'trace'");
    auto expr = expression();
    consume(TokenType::RightParen, "Expected ')' after expression");
    consume(TokenType::Semicolon, "Expected ';' after trace statement");
    
    return makeUnique<TraceStmt>(std::move(expr), loc);
}

Unique<Expr> Parser::expression() {
    return assignment();
}

Unique<Expr> Parser::assignment() {
    auto expr = logicalOr();
    
    if (match(TokenType::Equal)) {
        auto loc = previous_.getLocation();
        auto value = assignment();
        
        // For now, we'll handle assignment as a binary expression
        expr = makeUnique<BinaryExpr>(std::move(expr), TokenType::Equal,
                                      std::move(value), loc);
    }
    
    return expr;
}

Unique<Expr> Parser::logicalOr() {
    auto expr = logicalAnd();
    
    while (match(TokenType::PipePipe)) {
        auto op = previous_.getType();
        auto loc = previous_.getLocation();
        auto right = logicalAnd();
        expr = makeUnique<BinaryExpr>(std::move(expr), op, std::move(right), loc);
    }
    
    return expr;
}

Unique<Expr> Parser::logicalAnd() {
    auto expr = equality();
    
    while (match(TokenType::AmpAmp)) {
        auto op = previous_.getType();
        auto loc = previous_.getLocation();
        auto right = equality();
        expr = makeUnique<BinaryExpr>(std::move(expr), op, std::move(right), loc);
    }
    
    return expr;
}

Unique<Expr> Parser::equality() {
    auto expr = comparison();
    
    while (match(TokenType::EqualEqual, TokenType::BangEqual)) {
        auto op = previous_.getType();
        auto loc = previous_.getLocation();
        auto right = comparison();
        expr = makeUnique<BinaryExpr>(std::move(expr), op, std::move(right), loc);
    }
    
    return expr;
}

Unique<Expr> Parser::comparison() {
    auto expr = bitwiseOr();
    
    while (match(TokenType::Greater, TokenType::GreaterEqual,
                 TokenType::Less, TokenType::LessEqual)) {
        auto op = previous_.getType();
        auto loc = previous_.getLocation();
        auto right = bitwiseOr();
        expr = makeUnique<BinaryExpr>(std::move(expr), op, std::move(right), loc);
    }
    
    return expr;
}

Unique<Expr> Parser::bitwiseOr() {
    auto expr = bitwiseXor();
    
    while (match(TokenType::Pipe)) {
        auto op = previous_.getType();
        auto loc = previous_.getLocation();
        auto right = bitwiseXor();
        expr = makeUnique<BinaryExpr>(std::move(expr), op, std::move(right), loc);
    }
    
    return expr;
}

Unique<Expr> Parser::bitwiseXor() {
    auto expr = bitwiseAnd();
    
    while (match(TokenType::Caret)) {
        auto op = previous_.getType();
        auto loc = previous_.getLocation();
        auto right = bitwiseAnd();
        expr = makeUnique<BinaryExpr>(std::move(expr), op, std::move(right), loc);
    }
    
    return expr;
}

Unique<Expr> Parser::bitwiseAnd() {
    auto expr = shift();
    
    while (match(TokenType::Amp)) {
        auto op = previous_.getType();
        auto loc = previous_.getLocation();
        auto right = shift();
        expr = makeUnique<BinaryExpr>(std::move(expr), op, std::move(right), loc);
    }
    
    return expr;
}

Unique<Expr> Parser::shift() {
    auto expr = addition();
    
    while (match(TokenType::LessLess, TokenType::GreaterGreater)) {
        auto op = previous_.getType();
        auto loc = previous_.getLocation();
        auto right = addition();
        expr = makeUnique<BinaryExpr>(std::move(expr), op, std::move(right), loc);
    }
    
    return expr;
}

Unique<Expr> Parser::addition() {
    auto expr = multiplication();
    
    while (match(TokenType::Plus, TokenType::Minus)) {
        auto op = previous_.getType();
        auto loc = previous_.getLocation();
        auto right = multiplication();
        expr = makeUnique<BinaryExpr>(std::move(expr), op, std::move(right), loc);
    }
    
    return expr;
}

Unique<Expr> Parser::multiplication() {
    auto expr = unary();
    
    while (match(TokenType::Star, TokenType::Slash, TokenType::Percent)) {
        auto op = previous_.getType();
        auto loc = previous_.getLocation();
        auto right = unary();
        expr = makeUnique<BinaryExpr>(std::move(expr), op, std::move(right), loc);
    }
    
    return expr;
}

Unique<Expr> Parser::unary() {
    if (match(TokenType::Bang, TokenType::Minus, TokenType::Tilde)) {
        auto op = previous_.getType();
        auto loc = previous_.getLocation();
        auto expr = unary();
        return makeUnique<UnaryExpr>(op, std::move(expr), loc);
    }
    
    return postfix();
}

Unique<Expr> Parser::postfix() {
    return call();
}

Unique<Expr> Parser::call() {
    auto expr = primary();
    
    while (true) {
        if (match(TokenType::LeftParen)) {
            auto loc = previous_.getLocation();
            Vec<Unique<Expr>> args;
            
            if (!check(TokenType::RightParen)) {
                do {
                    args.push_back(expression());
                } while (match(TokenType::Comma));
            }
            
            consume(TokenType::RightParen, "Expected ')' after arguments");
            expr = makeUnique<CallExpr>(std::move(expr), std::move(args), loc);
        } else {
            break;
        }
    }
    
    return expr;
}

Unique<Expr> Parser::primary() {
    auto loc = current_.getLocation();
    
    if (match(TokenType::KwTrue)) {
        return makeUnique<BoolLiteral>(true, loc);
    }
    
    if (match(TokenType::KwFalse)) {
        return makeUnique<BoolLiteral>(false, loc);
    }
    
    if (match(TokenType::IntegerLiteral)) {
        String lexeme = previous_.getLexeme();
        int64_t value = std::stoll(lexeme);
        return makeUnique<IntegerLiteral>(value, loc);
    }
    
    if (match(TokenType::FloatLiteral)) {
        String lexeme = previous_.getLexeme();
        double value = std::stod(lexeme);
        return makeUnique<FloatLiteral>(value, loc);
    }
    
    if (match(TokenType::StringLiteral)) {
        String lexeme = previous_.getLexeme();
        // Remove quotes and process escape sequences
        String raw = lexeme.substr(1, lexeme.length() - 2);
        String value;
        
        for (size_t i = 0; i < raw.length(); i++) {
            if (raw[i] == '\\' && i + 1 < raw.length()) {
                switch (raw[i + 1]) {
                    case 'n': value += '\n'; i++; break;
                    case 't': value += '\t'; i++; break;
                    case 'r': value += '\r'; i++; break;
                    case '\\': value += '\\'; i++; break;
                    case '"': value += '"'; i++; break;
                    case '0': value += '\0'; i++; break;
                    default: value += raw[i]; break;
                }
            } else {
                value += raw[i];
            }
        }
        
        return makeUnique<StringLiteral>(value, loc);
    }
    
    if (match(TokenType::Identifier)) {
        String name = previous_.getLexeme();
        return makeUnique<Identifier>(name, loc);
    }
    
    if (match(TokenType::LeftParen)) {
        auto expr = expression();
        consume(TokenType::RightParen, "Expected ')' after expression");
        return expr;
    }
    
    error("Expected expression");
    // Return nullptr instead of dummy value to signal error
    return nullptr;
}

Unique<TypeNode> Parser::parseType() {
    auto loc = current_.getLocation();
    
    // Match type keywords (i8, i16, i32, etc.) or identifiers
    if (match(TokenType::KwI8, TokenType::KwI16, TokenType::KwI32, TokenType::KwI64,
              TokenType::KwU8, TokenType::KwU16, TokenType::KwU32, TokenType::KwU64,
              TokenType::KwF32, TokenType::KwF64, TokenType::KwBool, TokenType::KwChar,
              TokenType::KwStr, TokenType::KwVoid, TokenType::Identifier)) {
        String typeName = previous_.getLexeme();
        return makeUnique<TypeName>(typeName, loc);
    }
    
    error("Expected type name");
    return makeUnique<TypeName>("void", loc);
}

Vec<Parameter> Parser::parseParameters() {
    Vec<Parameter> params;
    
    if (!check(TokenType::RightParen)) {
        do {
            consume(TokenType::Identifier, "Expected parameter name");
            String name = previous_.getLexeme();
            auto loc = previous_.getLocation();
            
            consume(TokenType::Colon, "Expected ':' after parameter name");
            auto type = parseType();
            
            params.push_back(Parameter{name, std::move(type), loc});
        } while (match(TokenType::Comma));
    }
    
    return params;
}

} // namespace xypher


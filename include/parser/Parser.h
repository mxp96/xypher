#ifndef XYPHER_PARSER_H
#define XYPHER_PARSER_H

#include "Common.h"
#include "lexer/Lexer.h"
#include "lexer/Token.h"
#include "ast/AST.h"
#include "frontend/Diagnostics.h"

namespace xypher {

class Parser {
public:
    Parser(Lexer& lexer, DiagnosticEngine& diags);
    
    Unique<Program> parseProgram();
    
private:
    Lexer& lexer_;
    DiagnosticEngine& diags_;
    Token current_;
    Token previous_;
    
    // Helper methods
    void advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    template<typename... Ts>
    bool match(TokenType t1, TokenType t2, Ts... ts);
    bool consume(TokenType type, const String& message);
    void synchronize();
    
    Token peek() const { return current_; }
    Token prev() const { return previous_; }
    bool isAtEnd() const { return current_.is(TokenType::EndOfFile); }
    
    void error(const String& message);
    void errorAtCurrent(const String& message);
    void errorAtPrevious(const String& message);
    
    // Parsing methods
    Unique<Stmt> declaration();
    Unique<Stmt> funcDecl();
    Unique<Stmt> varDecl();
    Unique<Stmt> statement();
    Unique<Stmt> exprStatement();
    Unique<Stmt> blockStatement();
    Unique<Stmt> ifStatement();
    Unique<Stmt> returnStatement();
    Unique<Stmt> loopwhileStatement();
    Unique<Stmt> whileStatement();
    Unique<Stmt> forStatement();
    Unique<Stmt> sayStatement();
    Unique<Stmt> traceStatement();
    
    Unique<Expr> expression();
    Unique<Expr> assignment();
    Unique<Expr> logicalOr();
    Unique<Expr> logicalAnd();
    Unique<Expr> equality();
    Unique<Expr> comparison();
    Unique<Expr> bitwiseOr();
    Unique<Expr> bitwiseXor();
    Unique<Expr> bitwiseAnd();
    Unique<Expr> shift();
    Unique<Expr> addition();
    Unique<Expr> multiplication();
    Unique<Expr> unary();
    Unique<Expr> postfix();
    Unique<Expr> call();
    Unique<Expr> primary();
    
    Unique<TypeNode> parseType();
    Vec<Parameter> parseParameters();
};

template<typename... Ts>
bool Parser::match(TokenType t1, TokenType t2, Ts... ts) {
    if (match(t1)) return true;
    return match(t2, ts...);
}

} // namespace xypher

#endif


#ifndef XYPHER_AST_H
#define XYPHER_AST_H

#include "Common.h"
#include "lexer/Token.h"
#include "frontend/SourceLocation.h"

namespace xypher {

class ASTVisitor;

enum class ASTNodeType {
    // Expressions
    IntegerLiteral,
    FloatLiteral,
    StringLiteral,
    CharLiteral,
    BoolLiteral,
    NullLiteral,
    Identifier,
    BinaryExpr,
    UnaryExpr,
    CallExpr,
    IndexExpr,
    MemberExpr,
    CastExpr,
    
    // Statements
    ExprStmt,
    VarDecl,
    FuncDecl,
    ReturnStmt,
    IfStmt,
    LoopwhileStmt,
    ForStmt,
    WhileStmt,
    BreakStmt,
    ContinueStmt,
    BlockStmt,
    
    // Type nodes
    TypeName,
    PointerType,
    ArrayType,
    
    // Special
    SayStmt,
    GrabExpr,
    TraceStmt,
    FallStmt,
    
    // Top-level
    Program,
    LinkDirective,
};

class ASTNode {
public:
    explicit ASTNode(ASTNodeType type, SourceLocation loc)
        : type_(type), location_(loc) {}
    
    virtual ~ASTNode() = default;
    
    ASTNodeType getType() const { return type_; }
    const SourceLocation& getLocation() const { return location_; }
    
    virtual void accept(ASTVisitor& visitor) = 0;
    
protected:
    ASTNodeType type_;
    SourceLocation location_;
};

// Base classes
class Expr : public ASTNode {
public:
    explicit Expr(ASTNodeType type, SourceLocation loc)
        : ASTNode(type, loc) {}
};

class Stmt : public ASTNode {
public:
    explicit Stmt(ASTNodeType type, SourceLocation loc)
        : ASTNode(type, loc) {}
};

class TypeNode : public ASTNode {
public:
    explicit TypeNode(ASTNodeType type, SourceLocation loc)
        : ASTNode(type, loc) {}
    
    virtual String getTypeName() const = 0;
};

// Literal expressions
class IntegerLiteral : public Expr {
public:
    IntegerLiteral(int64_t value, SourceLocation loc)
        : Expr(ASTNodeType::IntegerLiteral, loc), value_(value) {}
    
    int64_t getValue() const { return value_; }
    void accept(ASTVisitor& visitor) override;
    
private:
    int64_t value_;
};

class FloatLiteral : public Expr {
public:
    FloatLiteral(double value, SourceLocation loc)
        : Expr(ASTNodeType::FloatLiteral, loc), value_(value) {}
    
    double getValue() const { return value_; }
    void accept(ASTVisitor& visitor) override;
    
private:
    double value_;
};

class StringLiteral : public Expr {
public:
    StringLiteral(String value, SourceLocation loc)
        : Expr(ASTNodeType::StringLiteral, loc), value_(std::move(value)) {}
    
    const String& getValue() const { return value_; }
    void accept(ASTVisitor& visitor) override;
    
private:
    String value_;
};

class BoolLiteral : public Expr {
public:
    BoolLiteral(bool value, SourceLocation loc)
        : Expr(ASTNodeType::BoolLiteral, loc), value_(value) {}
    
    bool getValue() const { return value_; }
    void accept(ASTVisitor& visitor) override;
    
private:
    bool value_;
};

class Identifier : public Expr {
public:
    Identifier(String name, SourceLocation loc)
        : Expr(ASTNodeType::Identifier, loc), name_(std::move(name)) {}
    
    const String& getName() const { return name_; }
    void accept(ASTVisitor& visitor) override;
    
private:
    String name_;
};

class BinaryExpr : public Expr {
public:
    BinaryExpr(Unique<Expr> left, TokenType op, Unique<Expr> right, SourceLocation loc)
        : Expr(ASTNodeType::BinaryExpr, loc),
          left_(std::move(left)), op_(op), right_(std::move(right)) {}
    
    Expr* getLeft() const { return left_.get(); }
    TokenType getOp() const { return op_; }
    Expr* getRight() const { return right_.get(); }
    void accept(ASTVisitor& visitor) override;
    
private:
    Unique<Expr> left_;
    TokenType op_;
    Unique<Expr> right_;
};

class UnaryExpr : public Expr {
public:
    UnaryExpr(TokenType op, Unique<Expr> operand, SourceLocation loc)
        : Expr(ASTNodeType::UnaryExpr, loc),
          op_(op), operand_(std::move(operand)) {}
    
    TokenType getOp() const { return op_; }
    Expr* getOperand() const { return operand_.get(); }
    void accept(ASTVisitor& visitor) override;
    
private:
    TokenType op_;
    Unique<Expr> operand_;
};

class CallExpr : public Expr {
public:
    CallExpr(Unique<Expr> callee, Vec<Unique<Expr>> args, SourceLocation loc)
        : Expr(ASTNodeType::CallExpr, loc),
          callee_(std::move(callee)), args_(std::move(args)) {}
    
    Expr* getCallee() const { return callee_.get(); }
    const Vec<Unique<Expr>>& getArgs() const { return args_; }
    void accept(ASTVisitor& visitor) override;
    
private:
    Unique<Expr> callee_;
    Vec<Unique<Expr>> args_;
};

// Type nodes
class TypeName : public TypeNode {
public:
    TypeName(String name, SourceLocation loc)
        : TypeNode(ASTNodeType::TypeName, loc), name_(std::move(name)) {}
    
    String getTypeName() const override { return name_; }
    void accept(ASTVisitor& visitor) override;
    
private:
    String name_;
};

// Statements
class ExprStmt : public Stmt {
public:
    ExprStmt(Unique<Expr> expr, SourceLocation loc)
        : Stmt(ASTNodeType::ExprStmt, loc), expr_(std::move(expr)) {}
    
    Expr* getExpr() const { return expr_.get(); }
    void accept(ASTVisitor& visitor) override;
    
private:
    Unique<Expr> expr_;
};

class VarDecl : public Stmt {
public:
    VarDecl(String name, Unique<TypeNode> type, Unique<Expr> init,
            bool isOwned, bool isConst, SourceLocation loc)
        : Stmt(ASTNodeType::VarDecl, loc),
          name_(std::move(name)), type_(std::move(type)),
          init_(std::move(init)), isOwned_(isOwned), isConst_(isConst) {}
    
    const String& getName() const { return name_; }
    TypeNode* getType() const { return type_.get(); }
    Expr* getInit() const { return init_.get(); }
    bool isOwned() const { return isOwned_; }
    bool isConst() const { return isConst_; }
    void accept(ASTVisitor& visitor) override;
    
private:
    String name_;
    Unique<TypeNode> type_;
    Unique<Expr> init_;
    bool isOwned_;
    bool isConst_;
};

class BlockStmt : public Stmt {
public:
    BlockStmt(Vec<Unique<Stmt>> stmts, SourceLocation loc)
        : Stmt(ASTNodeType::BlockStmt, loc), stmts_(std::move(stmts)) {}
    
    const Vec<Unique<Stmt>>& getStmts() const { return stmts_; }
    void accept(ASTVisitor& visitor) override;
    
private:
    Vec<Unique<Stmt>> stmts_;
};

class ReturnStmt : public Stmt {
public:
    ReturnStmt(Unique<Expr> value, SourceLocation loc)
        : Stmt(ASTNodeType::ReturnStmt, loc), value_(std::move(value)) {}
    
    Expr* getValue() const { return value_.get(); }
    void accept(ASTVisitor& visitor) override;
    
private:
    Unique<Expr> value_;
};

class IfStmt : public Stmt {
public:
    IfStmt(Unique<Expr> cond, Unique<Stmt> thenBranch,
           Unique<Stmt> elseBranch, SourceLocation loc)
        : Stmt(ASTNodeType::IfStmt, loc),
          cond_(std::move(cond)),
          thenBranch_(std::move(thenBranch)),
          elseBranch_(std::move(elseBranch)) {}
    
    Expr* getCond() const { return cond_.get(); }
    Stmt* getThenBranch() const { return thenBranch_.get(); }
    Stmt* getElseBranch() const { return elseBranch_.get(); }
    void accept(ASTVisitor& visitor) override;
    
private:
    Unique<Expr> cond_;
    Unique<Stmt> thenBranch_;
    Unique<Stmt> elseBranch_;
};

class LoopwhileStmt : public Stmt {
public:
    LoopwhileStmt(Unique<Expr> cond, Unique<Stmt> body, SourceLocation loc)
        : Stmt(ASTNodeType::LoopwhileStmt, loc),
          cond_(std::move(cond)), body_(std::move(body)) {}
    
    Expr* getCond() const { return cond_.get(); }
    Stmt* getBody() const { return body_.get(); }
    void accept(ASTVisitor& visitor) override;
    
private:
    Unique<Expr> cond_;
    Unique<Stmt> body_;
};

// Xypher-specific nodes
class SayStmt : public Stmt {
public:
    SayStmt(Vec<Unique<Expr>> exprs, SourceLocation loc)
        : Stmt(ASTNodeType::SayStmt, loc), exprs_(std::move(exprs)) {}
    
    const Vec<Unique<Expr>>& getExprs() const { return exprs_; }
    void accept(ASTVisitor& visitor) override;
    
private:
    Vec<Unique<Expr>> exprs_;
};

class TraceStmt : public Stmt {
public:
    TraceStmt(Unique<Expr> expr, SourceLocation loc)
        : Stmt(ASTNodeType::TraceStmt, loc), expr_(std::move(expr)) {}
    
    Expr* getExpr() const { return expr_.get(); }
    void accept(ASTVisitor& visitor) override;
    
private:
    Unique<Expr> expr_;
};

// Function declaration
struct Parameter {
    String name;
    Unique<TypeNode> type;
    SourceLocation location;
};

class FuncDecl : public Stmt {
public:
    FuncDecl(String name, Vec<Parameter> params,
             Unique<TypeNode> returnType, Unique<BlockStmt> body,
             SourceLocation loc)
        : Stmt(ASTNodeType::FuncDecl, loc),
          name_(std::move(name)), params_(std::move(params)),
          returnType_(std::move(returnType)), body_(std::move(body)) {}
    
    const String& getName() const { return name_; }
    const Vec<Parameter>& getParams() const { return params_; }
    TypeNode* getReturnType() const { return returnType_.get(); }
    BlockStmt* getBody() const { return body_.get(); }
    void accept(ASTVisitor& visitor) override;
    
private:
    String name_;
    Vec<Parameter> params_;
    Unique<TypeNode> returnType_;
    Unique<BlockStmt> body_;
};

// Program (top-level)
class Program : public ASTNode {
public:
    Program(Vec<Unique<Stmt>> decls, SourceLocation loc)
        : ASTNode(ASTNodeType::Program, loc), decls_(std::move(decls)) {}
    
    const Vec<Unique<Stmt>>& getDecls() const { return decls_; }
    void accept(ASTVisitor& visitor) override;
    
private:
    Vec<Unique<Stmt>> decls_;
};

} // namespace xypher

#endif


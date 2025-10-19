#ifndef XYPHER_TYPES_H
#define XYPHER_TYPES_H

#include "Common.h"
#include "ast/AST.h"

namespace xypher {

struct StructField {
    String name;
    Unique<TypeNode> type;
    SourceLocation location;
};

class StructDecl : public Stmt {
public:
    StructDecl(String name, Vec<StructField> fields, SourceLocation loc)
        : Stmt(ASTNodeType::FuncDecl, loc), 
          name_(std::move(name)), fields_(std::move(fields)) {}
    
    const String& getName() const { return name_; }
    const Vec<StructField>& getFields() const { return fields_; }
    void accept(ASTVisitor& visitor) override;
    
private:
    String name_;
    Vec<StructField> fields_;
};

struct EnumVariant {
    String name;
    Optional<int64_t> value;
    SourceLocation location;
};

class EnumDecl : public Stmt {
public:
    EnumDecl(String name, Vec<EnumVariant> variants, SourceLocation loc)
        : Stmt(ASTNodeType::FuncDecl, loc),
          name_(std::move(name)), variants_(std::move(variants)) {}
    
    const String& getName() const { return name_; }
    const Vec<EnumVariant>& getVariants() const { return variants_; }
    void accept(ASTVisitor& visitor) override;
    
private:
    String name_;
    Vec<EnumVariant> variants_;
};

class MatchExpr : public Expr {
public:
    struct MatchArm {
        Unique<Expr> pattern;
        Unique<Expr> body;
    };
    
    MatchExpr(Unique<Expr> value, Vec<MatchArm> arms, SourceLocation loc)
        : Expr(ASTNodeType::CallExpr, loc),
          value_(std::move(value)), arms_(std::move(arms)) {}
    
    Expr* getValue() const { return value_.get(); }
    const Vec<MatchArm>& getArms() const { return arms_; }
    void accept(ASTVisitor& visitor) override;
    
private:
    Unique<Expr> value_;
    Vec<MatchArm> arms_;
};

}

#endif


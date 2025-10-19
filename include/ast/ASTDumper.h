#ifndef XYPHER_AST_DUMPER_H
#define XYPHER_AST_DUMPER_H

#include "ast/ASTVisitor.h"
#include "Common.h"
#include <iostream>

namespace xypher {

// Forward declaration
class ASTNode;

class ASTDumper : public ASTVisitor {
public:
    explicit ASTDumper(std::ostream& out = std::cout) : out_(out) {}
    
    void dump(ASTNode* node);
    
    void visit(IntegerLiteral* node) override;
    void visit(FloatLiteral* node) override;
    void visit(StringLiteral* node) override;
    void visit(BoolLiteral* node) override;
    void visit(Identifier* node) override;
    void visit(BinaryExpr* node) override;
    void visit(UnaryExpr* node) override;
    void visit(CallExpr* node) override;
    
    void visit(TypeName* node) override;
    
    void visit(ExprStmt* node) override;
    void visit(VarDecl* node) override;
    void visit(BlockStmt* node) override;
    void visit(ReturnStmt* node) override;
    void visit(IfStmt* node) override;
    void visit(LoopwhileStmt* node) override;
    
    void visit(SayStmt* node) override;
    void visit(TraceStmt* node) override;
    
    void visit(FuncDecl* node) override;
    void visit(Program* node) override;
    
private:
    std::ostream& out_;
    int indent_ = 0;
    
    void printIndent();
    void increaseIndent() { indent_ += 2; }
    void decreaseIndent() { indent_ -= 2; }
};

} // namespace xypher

#endif


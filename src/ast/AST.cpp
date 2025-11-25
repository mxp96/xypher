#include "ast/AST.h"

#include "ast/ASTVisitor.h"

namespace xypher {

void IntegerLiteral::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}
void FloatLiteral::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}
void StringLiteral::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}
void BoolLiteral::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}
void Identifier::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}
void BinaryExpr::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}
void UnaryExpr::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}
void CallExpr::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

void TypeName::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

void ExprStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}
void VarDecl::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}
void BlockStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}
void ReturnStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}
void IfStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}
void LoopwhileStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

void SayStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}
void TraceStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

void ImportDecl::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}
void FuncDecl::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}
void Program::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

} // namespace xypher

#ifndef XYPHER_AST_VISITOR_H
#define XYPHER_AST_VISITOR_H

namespace xypher {

class IntegerLiteral;
class FloatLiteral;
class StringLiteral;
class BoolLiteral;
class Identifier;
class BinaryExpr;
class UnaryExpr;
class CallExpr;
class TypeName;
class ExprStmt;
class VarDecl;
class BlockStmt;
class ReturnStmt;
class IfStmt;
class LoopwhileStmt;
class SayStmt;
class TraceStmt;
class FuncDecl;
class Program;

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    virtual void visit(IntegerLiteral* node) = 0;
    virtual void visit(FloatLiteral* node) = 0;
    virtual void visit(StringLiteral* node) = 0;
    virtual void visit(BoolLiteral* node) = 0;
    virtual void visit(Identifier* node) = 0;
    virtual void visit(BinaryExpr* node) = 0;
    virtual void visit(UnaryExpr* node) = 0;
    virtual void visit(CallExpr* node) = 0;
    
    virtual void visit(TypeName* node) = 0;
    
    virtual void visit(ExprStmt* node) = 0;
    virtual void visit(VarDecl* node) = 0;
    virtual void visit(BlockStmt* node) = 0;
    virtual void visit(ReturnStmt* node) = 0;
    virtual void visit(IfStmt* node) = 0;
    virtual void visit(LoopwhileStmt* node) = 0;
    
    virtual void visit(SayStmt* node) = 0;
    virtual void visit(TraceStmt* node) = 0;
    
    virtual void visit(FuncDecl* node) = 0;
    virtual void visit(Program* node) = 0;
};

} // namespace xypher

#endif


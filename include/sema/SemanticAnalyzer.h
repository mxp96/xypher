#ifndef XYPHER_SEMANTIC_ANALYZER_H
#define XYPHER_SEMANTIC_ANALYZER_H

#include "Common.h"
#include "ast/AST.h"
#include "ast/ASTVisitor.h"
#include "frontend/Diagnostics.h"
#include "sema/SymbolTable.h"
#include "sema/ModuleRegistry.h"

namespace xypher {

class SemanticAnalyzer : public ASTVisitor {
  public:
    SemanticAnalyzer(DiagnosticEngine& diags);

    bool analyze(Program* program);

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

    void visit(ImportDecl* node) override;
    void visit(FuncDecl* node) override;
    void visit(Program* node) override;

    String getExprType(Expr* expr);

  private:
    DiagnosticEngine& diags_;
    SymbolTable symbols_;
    ModuleRegistry moduleRegistry_;
    Map<Expr*, String> exprTypes_;
    String currentFunction_;
    String currentReturnType_;

    void error(const String& message, const SourceLocation& loc);
    void warning(const String& message, const SourceLocation& loc);
    void registerBuiltinFunctions();
};

} // namespace xypher

#endif

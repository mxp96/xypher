#ifndef XYPHER_CODE_GENERATOR_H
#define XYPHER_CODE_GENERATOR_H

#include "Common.h"
#include "ast/AST.h"
#include "ast/ASTVisitor.h"
#include "frontend/Diagnostics.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>

namespace xypher {

class CodeGenerator : public ASTVisitor {
public:
    CodeGenerator(const String& moduleName, DiagnosticEngine& diags);
    ~CodeGenerator();
    
    bool generate(Program* program);
    
    llvm::Module* getModule() { return module_.get(); }
    
    void emitLLVMIR(const String& filename);
    bool compileToObject(const String& filename);
    bool linkToExecutable(const String& objFile, const String& exeFile);
    
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
    DiagnosticEngine& diags_;
    
    Unique<llvm::LLVMContext> context_;
    Unique<llvm::Module> module_;
    Unique<llvm::IRBuilder<>> builder_;
    
    Map<String, llvm::AllocaInst*> namedValues_;
    Map<String, llvm::GlobalVariable*> globalValues_;
    Map<String, llvm::Function*> functions_;
    
    llvm::Value* currentValue_ = nullptr;
    llvm::Function* currentFunction_ = nullptr;
    
    llvm::Type* getLLVMType(const String& typeName);
    llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* func,
                                            const String& varName,
                                            llvm::Type* type);
    
    void declarePrintf();
    void declareBuiltins();
    
    void error(const String& message);
};

} // namespace xypher

#endif


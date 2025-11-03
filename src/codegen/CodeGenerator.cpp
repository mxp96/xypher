#include "codegen/CodeGenerator.h"
#include "codegen/LLVMBackend.h"

#include <llvm/IR/Verifier.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

namespace xypher {

CodeGenerator::CodeGenerator(const String& moduleName, DiagnosticEngine& diags)
    : diags_(diags) {
    context_ = makeUnique<llvm::LLVMContext>();
    module_ = makeUnique<llvm::Module>(moduleName, *context_);
    builder_ = makeUnique<llvm::IRBuilder<>>(*context_);
}

CodeGenerator::~CodeGenerator() = default;

bool CodeGenerator::generate(Program* program) {
    try {
        declareBuiltins();
        program->accept(*this);
        
        if (llvm::verifyModule(*module_, &llvm::errs())) {
            error("Module verification failed");
            return false;
        }
        
        return !diags_.hasErrors();
    } catch (const std::exception& e) {
        error(String("Code generation exception: ") + e.what());
        return false;
    } catch (...) {
        error("Unknown code generation error");
        return false;
    }
}

void CodeGenerator::emitLLVMIR(const String& filename) {
    LLVMBackend::emitLLVMIR(module_.get(), filename);
}

bool CodeGenerator::compileToObject(const String& filename) {
    return LLVMBackend::emitObject(module_.get(), filename);
}

llvm::Type* CodeGenerator::getLLVMType(const String& typeName) {
    if (typeName == "i8") return llvm::Type::getInt8Ty(*context_);
    if (typeName == "i16") return llvm::Type::getInt16Ty(*context_);
    if (typeName == "i32") return llvm::Type::getInt32Ty(*context_);
    if (typeName == "i64") return llvm::Type::getInt64Ty(*context_);
    if (typeName == "u8") return llvm::Type::getInt8Ty(*context_);
    if (typeName == "u16") return llvm::Type::getInt16Ty(*context_);
    if (typeName == "u32") return llvm::Type::getInt32Ty(*context_);
    if (typeName == "u64") return llvm::Type::getInt64Ty(*context_);
    if (typeName == "f32") return llvm::Type::getFloatTy(*context_);
    if (typeName == "f64") return llvm::Type::getDoubleTy(*context_);
    if (typeName == "bool") return llvm::Type::getInt1Ty(*context_);
    if (typeName == "char") return llvm::Type::getInt8Ty(*context_);
    if (typeName == "str") return llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    if (typeName == "void") return llvm::Type::getVoidTy(*context_);
    
    return llvm::Type::getInt32Ty(*context_);
}

llvm::AllocaInst* CodeGenerator::createEntryBlockAlloca(
    llvm::Function* func, const String& varName, llvm::Type* type) {
    
    llvm::IRBuilder<> tmpBuilder(&func->getEntryBlock(),
                                 func->getEntryBlock().begin());
    return tmpBuilder.CreateAlloca(type, nullptr, varName);
}

void CodeGenerator::declarePrintf() {
    std::vector<llvm::Type*> printfArgs;
    printfArgs.push_back(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
    
    llvm::FunctionType* printfType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context_),
        printfArgs,
        true
    );
    
    llvm::Function::Create(
        printfType,
        llvm::Function::ExternalLinkage,
        "printf",
        module_.get()
    );
}

void CodeGenerator::declareBuiltins() {
    declarePrintf();
    
    // Time functions
    auto i64Type = llvm::Type::getInt64Ty(*context_);
    auto voidType = llvm::Type::getVoidTy(*context_);
    auto i32Type = llvm::Type::getInt32Ty(*context_);
    
    llvm::Function::Create(
        llvm::FunctionType::get(i64Type, false),
        llvm::Function::ExternalLinkage, "xy_time_ns", module_.get()
    );
    
    llvm::Function::Create(
        llvm::FunctionType::get(i64Type, false),
        llvm::Function::ExternalLinkage, "xy_time_us", module_.get()
    );
    
    llvm::Function::Create(
        llvm::FunctionType::get(i64Type, false),
        llvm::Function::ExternalLinkage, "xy_time_ms", module_.get()
    );
    
    llvm::Function::Create(
        llvm::FunctionType::get(i64Type, false),
        llvm::Function::ExternalLinkage, "xy_time_s", module_.get()
    );
    
    llvm::Function::Create(
        llvm::FunctionType::get(voidType, {i32Type}, false),
        llvm::Function::ExternalLinkage, "xy_sleep_ms", module_.get()
    );
}

void CodeGenerator::error(const String& message) {
    diags_.error(message, SourceLocation());
}

void CodeGenerator::visit(IntegerLiteral* node) {
    currentValue_ = llvm::ConstantInt::get(
        llvm::Type::getInt32Ty(*context_),
        node->getValue(),
        true
    );
}

void CodeGenerator::visit(FloatLiteral* node) {
    currentValue_ = llvm::ConstantFP::get(
        llvm::Type::getDoubleTy(*context_),
        node->getValue()
    );
}

void CodeGenerator::visit(StringLiteral* node) {
    // Create global string and get pointer to first element (i8*)
    llvm::Value* globalStr = builder_->CreateGlobalStringPtr(node->getValue(), "", 0, module_.get());
    currentValue_ = globalStr;
}

void CodeGenerator::visit(BoolLiteral* node) {
    currentValue_ = llvm::ConstantInt::get(
        llvm::Type::getInt1Ty(*context_),
        node->getValue() ? 1 : 0
    );
}

void CodeGenerator::visit(Identifier* node) {
    // Look for local variables first (function scope)
    auto localIt = namedValues_.find(node->getName());
    if (localIt != namedValues_.end()) {
        currentValue_ = builder_->CreateLoad(
            localIt->second->getAllocatedType(),
            localIt->second,
            node->getName()
        );
        return;
    }
    
    // Then check for global constants
    auto globalIt = globalValues_.find(node->getName());
    if (globalIt != globalValues_.end()) {
        currentValue_ = builder_->CreateLoad(
            globalIt->second->getValueType(),
            globalIt->second,
            node->getName()
        );
        return;
    }
    
    error("Unknown variable: " + node->getName());
    currentValue_ = nullptr;
}

void CodeGenerator::visit(BinaryExpr* node) {
    node->getLeft()->accept(*this);
    llvm::Value* left = currentValue_;
    
    node->getRight()->accept(*this);
    llvm::Value* right = currentValue_;
    
    if (!left || !right) {
        currentValue_ = nullptr;
        return;
    }
    
    switch (node->getOp()) {
        case TokenType::Plus:
            if (left->getType()->isIntegerTy()) {
                currentValue_ = builder_->CreateAdd(left, right, "addtmp");
            } else {
                currentValue_ = builder_->CreateFAdd(left, right, "addtmp");
            }
            break;
        
        case TokenType::Minus:
            if (left->getType()->isIntegerTy()) {
                currentValue_ = builder_->CreateSub(left, right, "subtmp");
            } else {
                currentValue_ = builder_->CreateFSub(left, right, "subtmp");
            }
            break;
        
        case TokenType::Star:
            if (left->getType()->isIntegerTy()) {
                currentValue_ = builder_->CreateMul(left, right, "multmp");
            } else {
                currentValue_ = builder_->CreateFMul(left, right, "multmp");
            }
            break;
        
        case TokenType::Slash:
            if (left->getType()->isIntegerTy()) {
                currentValue_ = builder_->CreateSDiv(left, right, "divtmp");
            } else {
                currentValue_ = builder_->CreateFDiv(left, right, "divtmp");
            }
            break;
        
        case TokenType::Percent:
            currentValue_ = builder_->CreateSRem(left, right, "modtmp");
            break;
        
        case TokenType::EqualEqual:
            if (left->getType()->isIntegerTy()) {
                currentValue_ = builder_->CreateICmpEQ(left, right, "eqtmp");
            } else {
                currentValue_ = builder_->CreateFCmpOEQ(left, right, "eqtmp");
            }
            break;
        
        case TokenType::BangEqual:
            if (left->getType()->isIntegerTy()) {
                currentValue_ = builder_->CreateICmpNE(left, right, "netmp");
            } else {
                currentValue_ = builder_->CreateFCmpONE(left, right, "netmp");
            }
            break;
        
        case TokenType::Less:
            if (left->getType()->isIntegerTy()) {
                currentValue_ = builder_->CreateICmpSLT(left, right, "lttmp");
            } else {
                currentValue_ = builder_->CreateFCmpOLT(left, right, "lttmp");
            }
            break;
        
        case TokenType::LessEqual:
            if (left->getType()->isIntegerTy()) {
                currentValue_ = builder_->CreateICmpSLE(left, right, "letmp");
            } else {
                currentValue_ = builder_->CreateFCmpOLE(left, right, "letmp");
            }
            break;
        
        case TokenType::Greater:
            if (left->getType()->isIntegerTy()) {
                currentValue_ = builder_->CreateICmpSGT(left, right, "gttmp");
            } else {
                currentValue_ = builder_->CreateFCmpOGT(left, right, "gttmp");
            }
            break;
        
        case TokenType::GreaterEqual:
            if (left->getType()->isIntegerTy()) {
                currentValue_ = builder_->CreateICmpSGE(left, right, "getmp");
            } else {
                currentValue_ = builder_->CreateFCmpOGE(left, right, "getmp");
            }
            break;
        
        case TokenType::AmpAmp:
            currentValue_ = builder_->CreateAnd(left, right, "andtmp");
            break;
        
        case TokenType::PipePipe:
            currentValue_ = builder_->CreateOr(left, right, "ortmp");
            break;
        
        case TokenType::Amp:
            currentValue_ = builder_->CreateAnd(left, right, "bitandtmp");
            break;
        
        case TokenType::Pipe:
            currentValue_ = builder_->CreateOr(left, right, "bitortmp");
            break;
        
        case TokenType::Caret:
            currentValue_ = builder_->CreateXor(left, right, "xortmp");
            break;
        
        case TokenType::LessLess:
            currentValue_ = builder_->CreateShl(left, right, "shltmp");
            break;
        
        case TokenType::GreaterGreater:
            currentValue_ = builder_->CreateAShr(left, right, "shrtmp");
            break;
        
        case TokenType::Equal: {
            auto* ident = dynamic_cast<Identifier*>(node->getLeft());
            if (!ident) {
                error("Left side of assignment must be an identifier");
                currentValue_ = nullptr;
                return;
            }
            
            auto it = namedValues_.find(ident->getName());
            if (it == namedValues_.end()) {
                error("Unknown variable: " + ident->getName());
                currentValue_ = nullptr;
                return;
            }
            
            builder_->CreateStore(right, it->second);
            currentValue_ = right;
            break;
        }
        
        default:
            error("Unsupported binary operator");
            currentValue_ = nullptr;
            break;
    }
}

void CodeGenerator::visit(UnaryExpr* node) {
    node->getOperand()->accept(*this);
    llvm::Value* operand = currentValue_;
    
    if (!operand) {
        currentValue_ = nullptr;
        return;
    }
    
    switch (node->getOp()) {
        case TokenType::Minus:
            if (operand->getType()->isIntegerTy()) {
                currentValue_ = builder_->CreateNeg(operand, "negtmp");
            } else {
                currentValue_ = builder_->CreateFNeg(operand, "negtmp");
            }
            break;
        
        case TokenType::Bang:
            currentValue_ = builder_->CreateNot(operand, "nottmp");
            break;
        
        case TokenType::Tilde:
            currentValue_ = builder_->CreateNot(operand, "bitnottmp");
            break;
        
        default:
            error("Unsupported unary operator");
            currentValue_ = nullptr;
            break;
    }
}

void CodeGenerator::visit(CallExpr* node) {
    auto* identExpr = dynamic_cast<Identifier*>(node->getCallee());
    if (!identExpr) {
        error("Function call requires an identifier");
        currentValue_ = nullptr;
        return;
    }
    
    String funcName = identExpr->getName();
    llvm::Function* func = module_->getFunction(funcName);
    
    if (!func) {
        error("Unknown function: " + funcName);
        currentValue_ = nullptr;
        return;
    }
    
    std::vector<llvm::Value*> args;
    for (const auto& arg : node->getArgs()) {
        arg->accept(*this);
        if (currentValue_) {
            args.push_back(currentValue_);
        }
    }
    
    // Don't give name to void function calls
    if (func->getReturnType()->isVoidTy()) {
        builder_->CreateCall(func, args);
        currentValue_ = nullptr;
    } else {
        currentValue_ = builder_->CreateCall(func, args, "calltmp");
    }
}

void CodeGenerator::visit(TypeName* node) {
    // Types are handled elsewhere
}

void CodeGenerator::visit(ExprStmt* node) {
    node->getExpr()->accept(*this);
}

void CodeGenerator::visit(VarDecl* node) {
    if (!currentFunction_) {
        String typeName = node->getType() ? node->getType()->getTypeName() : "i32";
        llvm::Type* type = getLLVMType(typeName);
        
        llvm::Constant* initializer = llvm::Constant::getNullValue(type);
        
        if (node->getInit()) {
            node->getInit()->accept(*this);
            if (currentValue_) {
                if (auto* constant = llvm::dyn_cast<llvm::Constant>(currentValue_)) {
                    initializer = constant;
                }
            }
        }
        
        auto* globalVar = new llvm::GlobalVariable(
            *module_,
            type,
            node->isConst(),
            llvm::GlobalValue::InternalLinkage,
            initializer,
            node->getName()
        );
        
        globalValues_[node->getName()] = globalVar;
        
        return;
    }
    
    String typeName = node->getType() ? node->getType()->getTypeName() : "i32";
    llvm::Type* type = getLLVMType(typeName);
    
    llvm::AllocaInst* alloca = createEntryBlockAlloca(
        currentFunction_, node->getName(), type
    );
    
    if (node->getInit()) {
        node->getInit()->accept(*this);
        if (currentValue_) {
            builder_->CreateStore(currentValue_, alloca);
        }
    }
    
    namedValues_[node->getName()] = alloca;
}

void CodeGenerator::visit(BlockStmt* node) {
    for (const auto& stmt : node->getStmts()) {
        stmt->accept(*this);
    }
}

void CodeGenerator::visit(ReturnStmt* node) {
    if (node->getValue()) {
        node->getValue()->accept(*this);
        builder_->CreateRet(currentValue_);
    } else {
        builder_->CreateRetVoid();
    }
}

void CodeGenerator::visit(IfStmt* node) {
    node->getCond()->accept(*this);
    llvm::Value* condValue = currentValue_;
    
    if (!condValue) return;
    
    condValue = builder_->CreateICmpNE(
        condValue,
        llvm::ConstantInt::get(condValue->getType(), 0),
        "ifcond"
    );
    
    llvm::Function* func = builder_->GetInsertBlock()->getParent();
    
    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(*context_, "then", func);
    llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(*context_, "else");
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context_, "ifcont");
    
    if (node->getElseBranch()) {
        builder_->CreateCondBr(condValue, thenBB, elseBB);
    } else {
        builder_->CreateCondBr(condValue, thenBB, mergeBB);
    }
    
    builder_->SetInsertPoint(thenBB);
    node->getThenBranch()->accept(*this);
    if (!builder_->GetInsertBlock()->getTerminator()) {
        builder_->CreateBr(mergeBB);
    }
    thenBB = builder_->GetInsertBlock();
    
    if (node->getElseBranch()) {
        elseBB->insertInto(func);
        builder_->SetInsertPoint(elseBB);
        node->getElseBranch()->accept(*this);
        if (!builder_->GetInsertBlock()->getTerminator()) {
            builder_->CreateBr(mergeBB);
        }
        elseBB = builder_->GetInsertBlock();
    }
    
    mergeBB->insertInto(func);
    builder_->SetInsertPoint(mergeBB);
}

void CodeGenerator::visit(LoopwhileStmt* node) {
    llvm::Function* func = builder_->GetInsertBlock()->getParent();
    
    llvm::BasicBlock* condBB = llvm::BasicBlock::Create(*context_, "loopcond", func);
    llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(*context_, "loop");
    llvm::BasicBlock* afterBB = llvm::BasicBlock::Create(*context_, "afterloop");
    
    builder_->CreateBr(condBB);
    builder_->SetInsertPoint(condBB);
    
    node->getCond()->accept(*this);
    llvm::Value* condValue = currentValue_;
    
    if (condValue) {
        condValue = builder_->CreateICmpNE(
            condValue,
            llvm::ConstantInt::get(condValue->getType(), 0),
            "loopcond"
        );
        builder_->CreateCondBr(condValue, loopBB, afterBB);
    }
    
    loopBB->insertInto(func);
    builder_->SetInsertPoint(loopBB);
    
    node->getBody()->accept(*this);
    
    if (!builder_->GetInsertBlock()->getTerminator()) {
        builder_->CreateBr(condBB);
    }
    
    afterBB->insertInto(func);
    builder_->SetInsertPoint(afterBB);
}

void CodeGenerator::visit(SayStmt* node) {
    llvm::Function* printfFunc = module_->getFunction("printf");
    if (!printfFunc) {
        error("printf function not found");
        return;
    }
    
    // Print all expressions on one line, space-separated
    for (size_t i = 0; i < node->getExprs().size(); i++) {
        const auto& expr = node->getExprs()[i];
        expr->accept(*this);
        
        if (!currentValue_) continue;
        
        std::vector<llvm::Value*> args;
        
        // Add space before each arg except the first
        if (i > 0) {
            llvm::Value* spaceStr = builder_->CreateGlobalStringPtr(" ", "", 0, module_.get());
            std::vector<llvm::Value*> spaceArgs = {spaceStr};
            builder_->CreateCall(printfFunc, spaceArgs);
        }
        
        // Print the value without newline
        if (currentValue_->getType()->isIntegerTy()) {
            llvm::Value* formatStr = builder_->CreateGlobalStringPtr("%d", "", 0, module_.get());
            args.push_back(formatStr);
            args.push_back(currentValue_);
        } else if (currentValue_->getType()->isFloatingPointTy()) {
            llvm::Value* formatStr = builder_->CreateGlobalStringPtr("%f", "", 0, module_.get());
            args.push_back(formatStr);
            args.push_back(currentValue_);
        } else if (currentValue_->getType()->isPointerTy()) {
            llvm::Value* formatStr = builder_->CreateGlobalStringPtr("%s", "", 0, module_.get());
            args.push_back(formatStr);
            args.push_back(currentValue_);
        } else {
            llvm::Value* formatStr = builder_->CreateGlobalStringPtr("<value>", "", 0, module_.get());
            args.push_back(formatStr);
        }
        
        builder_->CreateCall(printfFunc, args);
    }
    
    // Print newline at the end
    llvm::Value* newlineStr = builder_->CreateGlobalStringPtr("\n", "", 0, module_.get());
    std::vector<llvm::Value*> newlineArgs = {newlineStr};
    builder_->CreateCall(printfFunc, newlineArgs);
}

void CodeGenerator::visit(TraceStmt* node) {
    // Similar to say, but could include type information
    node->getExpr()->accept(*this);
    
    if (!currentValue_) return;
    
    llvm::Function* printfFunc = module_->getFunction("printf");
    if (!printfFunc) return;
    
    std::vector<llvm::Value*> args;
    llvm::Value* formatStr = builder_->CreateGlobalStringPtr("[TRACE] %d\n", "", 0, module_.get());
    args.push_back(formatStr);
    
    if (currentValue_->getType()->isIntegerTy()) {
        args.push_back(currentValue_);
    } else {
        args.push_back(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0));
    }
    
    builder_->CreateCall(printfFunc, args);
}

void CodeGenerator::visit(FuncDecl* node) {
    try {
        String returnTypeName = node->getReturnType() ? 
                               node->getReturnType()->getTypeName() : "void";
        llvm::Type* returnType = getLLVMType(returnTypeName);
        
        std::vector<llvm::Type*> paramTypes;
        for (const auto& param : node->getParams()) {
            String paramTypeName = param.type ? param.type->getTypeName() : "i32";
            paramTypes.push_back(getLLVMType(paramTypeName));
        }
        
        llvm::FunctionType* funcType = llvm::FunctionType::get(
            returnType, paramTypes, false
        );
        
        llvm::Function* func = llvm::Function::Create(
            funcType,
            llvm::Function::ExternalLinkage,
            node->getName(),
            module_.get()
        );
        
        functions_[node->getName()] = func;
        
        size_t idx = 0;
        for (auto& arg : func->args()) {
            arg.setName(node->getParams()[idx].name);
            idx++;
        }
        
        llvm::BasicBlock* bb = llvm::BasicBlock::Create(*context_, "entry", func);
        builder_->SetInsertPoint(bb);
        
        currentFunction_ = func;
        namedValues_.clear();
        
        idx = 0;
        for (auto& arg : func->args()) {
            llvm::AllocaInst* alloca = createEntryBlockAlloca(
                func, 
                String(arg.getName()), 
                arg.getType()
            );
            builder_->CreateStore(&arg, alloca);
            namedValues_[String(arg.getName())] = alloca;
            idx++;
        }
        
        if (node->getBody()) {
            node->getBody()->accept(*this);
        }
        
        if (!builder_->GetInsertBlock()->getTerminator()) {
            if (returnType->isVoidTy()) {
                builder_->CreateRetVoid();
            } else {
                builder_->CreateRet(llvm::Constant::getNullValue(returnType));
            }
        }
        
        if (llvm::verifyFunction(*func, &llvm::errs())) {
            error("Function verification failed: " + node->getName());
            func->eraseFromParent();
        }
    } catch (const std::exception& e) {
        error("Exception in function " + node->getName() + ": " + e.what());
    }
}

void CodeGenerator::visit(Program* node) {
    for (const auto& decl : node->getDecls()) {
        decl->accept(*this);
    }
}

} // namespace xypher


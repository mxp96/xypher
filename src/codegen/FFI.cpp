#include "codegen/FFI.h"
#include <llvm/IR/DerivedTypes.h>

namespace xypher {

FFIManager::FFIManager(llvm::Module* module, llvm::LLVMContext* context, llvm::IRBuilder<>* builder)
    : module_(module), context_(context), builder_(builder) {}

llvm::Type* FFIManager::getTypeFromString(const String& typeName) {
    if (typeName == "i32") return llvm::Type::getInt32Ty(*context_);
    if (typeName == "i64") return llvm::Type::getInt64Ty(*context_);
    if (typeName == "f64") return llvm::Type::getDoubleTy(*context_);
    if (typeName == "str") return llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    if (typeName == "void") return llvm::Type::getVoidTy(*context_);
    return llvm::Type::getInt32Ty(*context_);
}

void FFIManager::declareExternalFunction(const ExternalFunction& func) {
    llvm::Type* returnType = getTypeFromString(func.returnType);
    
    std::vector<llvm::Type*> paramTypes;
    for (const auto& paramType : func.paramTypes) {
        paramTypes.push_back(getTypeFromString(paramType));
    }
    
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        returnType, paramTypes, func.isVariadic
    );
    
    llvm::Function* llvmFunc = llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage,
        func.name,
        module_
    );
    
    externalFunctions_[func.name] = llvmFunc;
}

llvm::Function* FFIManager::getExternalFunction(const String& name) {
    auto it = externalFunctions_.find(name);
    if (it != externalFunctions_.end()) {
        return it->second;
    }
    return module_->getFunction(name);
}

void FFIManager::declareStandardLibrary() {
    // Time functions
    declareExternalFunction({"xy_time_ms", "i64", {}, false});
    declareExternalFunction({"xy_time_us", "i64", {}, false});
    declareExternalFunction({"xy_time_ns", "i64", {}, false});
    declareExternalFunction({"xy_time_s", "i64", {}, false});
    declareExternalFunction({"xy_sleep_ms", "void", {"i32"}, false});
    
    // I/O functions
    declareExternalFunction({"xy_say_i32", "void", {"i32"}, false});
    declareExternalFunction({"xy_say_i64", "void", {"i64"}, false});
    declareExternalFunction({"xy_say_f64", "void", {"f64"}, false});
    declareExternalFunction({"xy_say_str", "void", {"str"}, false});
    declareExternalFunction({"xy_say_newline", "void", {}, false});
    
    // Math functions
    declareExternalFunction({"xy_sqrt", "f64", {"f64"}, false});
    declareExternalFunction({"xy_pow", "f64", {"f64", "f64"}, false});
    declareExternalFunction({"xy_sin", "f64", {"f64"}, false});
    declareExternalFunction({"xy_cos", "f64", {"f64"}, false});
    declareExternalFunction({"xy_abs_i32", "i32", {"i32"}, false});
    declareExternalFunction({"xy_abs_f64", "f64", {"f64"}, false});
    
    // Printf (legacy)
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
        module_
    );
}

}


#ifndef XYPHER_FFI_H
#define XYPHER_FFI_H

#include "Common.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

namespace xypher {

struct ExternalFunction {
    String name;
    String returnType;
    Vec<String> paramTypes;
    bool isVariadic;
};

class FFIManager {
public:
    FFIManager(llvm::Module* module, llvm::LLVMContext* context, llvm::IRBuilder<>* builder);
    
    void declareExternalFunction(const ExternalFunction& func);
    llvm::Function* getExternalFunction(const String& name);
    
    void declareStandardLibrary();
    
private:
    llvm::Module* module_;
    llvm::LLVMContext* context_;
    llvm::IRBuilder<>* builder_;
    Map<String, llvm::Function*> externalFunctions_;
    
    llvm::Type* getTypeFromString(const String& typeName);
};

}

#endif


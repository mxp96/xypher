#include "backend/JIT.h"
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/Support/TargetSelect.h>

namespace xypher {

JITEngine::JITEngine() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    
    auto jitOrErr = llvm::orc::LLJITBuilder().create();
    if (!jitOrErr) {
        jit_ = nullptr;
        return;
    }
    
    jit_ = std::move(*jitOrErr);
}

JITEngine::~JITEngine() = default;

bool JITEngine::addModule(Unique<llvm::Module> module) {
    if (!jit_) return false;
    
    auto err = jit_->addIRModule(llvm::orc::ThreadSafeModule(
        std::move(module), 
        std::make_unique<llvm::LLVMContext>()
    ));
    
    return !err;
}

void* JITEngine::lookup(const String& name) {
    if (!jit_) return nullptr;
    
    auto symbol = jit_->lookup(name);
    if (!symbol) {
        return nullptr;
    }
    
    return symbol->toPtr<void*>();
}

}


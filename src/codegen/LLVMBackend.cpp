#include "codegen/LLVMBackend.h"
#include "backend/TargetMachine.h"

#include <llvm/IR/Module.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/TargetSelect.h>

#include <system_error>

namespace xypher {

bool LLVMBackend::emitLLVMIR(llvm::Module* module, const String& filename) {
    std::error_code ec;
    llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);
    
    if (ec) {
        return false;
    }
    
    module->print(dest, nullptr);
    dest.flush();
    
    return true;
}

bool LLVMBackend::emitAssembly(llvm::Module* module, const String& filename) {
    TargetMachineManager::initialize();
    
    auto* targetMachine = TargetMachineManager::createTargetMachine();
    if (!targetMachine) {
        return false;
    }
    
    std::error_code ec;
    llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);
    
    if (ec) {
        return false;
    }
    
    llvm::legacy::PassManager pass;

    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr,
                                          llvm::CGFT_AssemblyFile)) {
        return false;
    }
    
    pass.run(*module);
    dest.flush();
    
    return true;
}

bool LLVMBackend::emitObject(llvm::Module* module, const String& filename) {
    TargetMachineManager::initialize();
    
    auto* targetMachine = TargetMachineManager::createTargetMachine();
    if (!targetMachine) {
        return false;
    }
    
    module->setDataLayout(targetMachine->createDataLayout());
    
    std::error_code ec;
    llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);
    
    if (ec) {
        return false;
    }
    
    llvm::legacy::PassManager pass;
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr,
                                          llvm::CGFT_ObjectFile)) {
        return false;
    }
    
    pass.run(*module);
    dest.flush();
    
    return true;
}

bool LLVMBackend::optimize(llvm::Module* module, int optLevel) {
    // Suppress unused parameter warnings
    (void)module;
    (void)optLevel;
    
    // Optimization passes would go here
    // This is a placeholder - actual optimization is done in Optimizer.cpp
    return true;
}

} // namespace xypher


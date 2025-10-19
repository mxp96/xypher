#include "backend/TargetMachine.h"

#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Config/llvm-config.h>

// Try to include Host header from different locations depending on LLVM version
#if LLVM_VERSION_MAJOR >= 15
    #include <llvm/TargetParser/Host.h>
#else
    #include <llvm/Support/Host.h>
#endif

// X86 target initialization
extern "C" {
    void LLVMInitializeX86TargetInfo();
    void LLVMInitializeX86Target();
    void LLVMInitializeX86TargetMC();
    void LLVMInitializeX86AsmParser();
    void LLVMInitializeX86AsmPrinter();
}

namespace xypher {

void TargetMachineManager::initialize() {
    LLVMInitializeX86TargetInfo();
    LLVMInitializeX86Target();
    LLVMInitializeX86TargetMC();
    LLVMInitializeX86AsmParser();
    LLVMInitializeX86AsmPrinter();
}

llvm::TargetMachine* TargetMachineManager::createTargetMachine(
    const String& targetTriple,
    const String& cpu,
    const String& features) {
    
    String triple = targetTriple.empty() ? 
                   llvm::sys::getDefaultTargetTriple() : targetTriple;
    
    std::string error;
    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(triple, error);
    
    if (!target) {
        return nullptr;
    }
    
    String cpuStr = cpu.empty() ? "generic" : cpu;
    String featuresStr = features;
    
    llvm::TargetOptions opt;
    llvm::TargetMachine* targetMachine = target->createTargetMachine(
        triple,
        cpuStr,
        featuresStr,
        opt,
        llvm::Reloc::PIC_
    );
    
    return targetMachine;
}

} // namespace xypher


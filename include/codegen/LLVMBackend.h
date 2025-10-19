#ifndef XYPHER_LLVM_BACKEND_H
#define XYPHER_LLVM_BACKEND_H

#include "Common.h"
#include <llvm/IR/Module.h>

namespace xypher {

class LLVMBackend {
public:
    static bool emitLLVMIR(llvm::Module* module, const String& filename);
    static bool emitAssembly(llvm::Module* module, const String& filename);
    static bool emitObject(llvm::Module* module, const String& filename);
    static bool optimize(llvm::Module* module, int optLevel);
};

} // namespace xypher

#endif


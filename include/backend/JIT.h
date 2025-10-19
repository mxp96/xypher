#ifndef XYPHER_JIT_H
#define XYPHER_JIT_H

#include "Common.h"
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/IR/Module.h>

namespace xypher {

class JITEngine {
public:
    JITEngine();
    ~JITEngine();
    
    bool addModule(Unique<llvm::Module> module);
    void* lookup(const String& name);
    
    template<typename FuncType>
    FuncType getFunction(const String& name) {
        return reinterpret_cast<FuncType>(lookup(name));
    }
    
private:
    Unique<llvm::orc::LLJIT> jit_;
};

}

#endif


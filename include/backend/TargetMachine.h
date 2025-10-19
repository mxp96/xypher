#ifndef XYPHER_TARGET_MACHINE_H
#define XYPHER_TARGET_MACHINE_H

#include "Common.h"
#include <llvm/Target/TargetMachine.h>

namespace xypher {

class TargetMachineManager {
public:
    static void initialize();
    static llvm::TargetMachine* createTargetMachine(
        const String& targetTriple = "",
        const String& cpu = "",
        const String& features = ""
    );
};

} // namespace xypher

#endif


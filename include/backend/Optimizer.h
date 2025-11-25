#ifndef XYPHER_OPTIMIZER_H
#define XYPHER_OPTIMIZER_H

#include "Common.h"

#include <llvm/IR/Module.h>

namespace xypher {

enum class OptimizationLevel {
    None = 0,
    O1 = 1,
    O2 = 2,
    O3 = 3,
    Os = 4, // Size optimization
    Oz = 5  // Aggressive size optimization
};

class Optimizer {
  public:
    static void optimize(llvm::Module* module, OptimizationLevel level);
    static void optimizeWithPipeline(llvm::Module* module, OptimizationLevel level);
    static bool verifyModule(llvm::Module* module, bool fatal = false);
    static void printOptimizationStats(llvm::Module* module);
};

} // namespace xypher

#endif

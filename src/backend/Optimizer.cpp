#include "backend/Optimizer.h"

#include <llvm/Passes/PassBuilder.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Transforms/IPO/AlwaysInliner.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>
#include <llvm/Transforms/Scalar/DCE.h>
#include <llvm/Transforms/Scalar/SROA.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Utils/Mem2Reg.h>

namespace xypher {

void Optimizer::optimize(llvm::Module* module, OptimizationLevel level) {
    if (level == OptimizationLevel::None) return;
    
    llvm::LoopAnalysisManager LAM;
    llvm::FunctionAnalysisManager FAM;
    llvm::CGSCCAnalysisManager CGAM;
    llvm::ModuleAnalysisManager MAM;
    
    llvm::PassBuilder PB;
    PB.registerModuleAnalyses(MAM);
    PB.registerCGSCCAnalyses(CGAM);
    PB.registerFunctionAnalyses(FAM);
    PB.registerLoopAnalyses(LAM);
    PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);
    
    llvm::OptimizationLevel llvmLevel;
    switch (level) {
        case OptimizationLevel::O1:
            llvmLevel = llvm::OptimizationLevel::O1;
            break;
        case OptimizationLevel::O2:
            llvmLevel = llvm::OptimizationLevel::O2;
            break;
        case OptimizationLevel::O3:
            llvmLevel = llvm::OptimizationLevel::O3;
            break;
        case OptimizationLevel::Os:
            llvmLevel = llvm::OptimizationLevel::Os;
            break;
        case OptimizationLevel::Oz:
            llvmLevel = llvm::OptimizationLevel::Oz;
            break;
        default:
            return;
    }
    
    llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(llvmLevel);
    MPM.run(*module, MAM);
}


} // namespace xypher


#include "backend/Optimizer.h"

#include <llvm/Passes/PassBuilder.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/Analysis/CGSCCPassManager.h>

namespace xypher {

void Optimizer::optimize(llvm::Module* module, OptimizationLevel level) {
    switch (level) {
        case OptimizationLevel::None:
            return;
        case OptimizationLevel::O1:
            applyO1Passes(module);
            break;
        case OptimizationLevel::O2:
            applyO2Passes(module);
            break;
        case OptimizationLevel::O3:
            applyO3Passes(module);
            break;
    }
}

void Optimizer::applyO1Passes(llvm::Module* module) {
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
    
    llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(
        llvm::OptimizationLevel::O1
    );
    
    MPM.run(*module, MAM);
}

void Optimizer::applyO2Passes(llvm::Module* module) {
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
    
    llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(
        llvm::OptimizationLevel::O2
    );
    
    MPM.run(*module, MAM);
}

void Optimizer::applyO3Passes(llvm::Module* module) {
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
    
    llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(
        llvm::OptimizationLevel::O3
    );
    
    MPM.run(*module, MAM);
}

} // namespace xypher


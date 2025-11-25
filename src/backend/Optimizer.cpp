#include "backend/Optimizer.h"

#include <iostream>
#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/IPO/AlwaysInliner.h>
#include <llvm/Transforms/IPO/DeadArgumentElimination.h>
#include <llvm/Transforms/IPO/FunctionAttrs.h>
#include <llvm/Transforms/IPO/GlobalOpt.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar/ADCE.h>
#include <llvm/Transforms/Scalar/DCE.h>
#include <llvm/Transforms/Scalar/EarlyCSE.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar/IndVarSimplify.h>
#include <llvm/Transforms/Scalar/LoopDeletion.h>
#include <llvm/Transforms/Scalar/LoopRotation.h>
#include <llvm/Transforms/Scalar/Reassociate.h>
#include <llvm/Transforms/Scalar/SCCP.h>
#include <llvm/Transforms/Scalar/SROA.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>
#include <llvm/Transforms/Utils/Mem2Reg.h>
#include <llvm/Transforms/Utils/PromoteMemToReg.h>
#include <llvm/Transforms/Vectorize/LoopVectorize.h>
#include <llvm/Transforms/Vectorize/SLPVectorizer.h>

namespace xypher {

void Optimizer::optimize(llvm::Module* module, OptimizationLevel level) {
    if (level == OptimizationLevel::None)
        return;

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

void Optimizer::optimizeWithPipeline(llvm::Module* module, OptimizationLevel level) {
    if (level == OptimizationLevel::None)
        return;

    if (!verifyModule(module, false)) {
        std::cerr << "[Optimizer] Warning: IR verification failed before optimization (non-fatal)"
                  << std::endl;
    }

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

    llvm::ModulePassManager MPM;
    llvm::FunctionPassManager earlyFPM;
    llvm::FunctionPassManager midFPM;
    llvm::FunctionPassManager lateFPM;

    earlyFPM.addPass(llvm::PromotePass());
    earlyFPM.addPass(llvm::SROAPass(llvm::SROAOptions::ModifyCFG));
    earlyFPM.addPass(llvm::EarlyCSEPass(true));
    earlyFPM.addPass(llvm::SimplifyCFGPass());
    earlyFPM.addPass(llvm::InstCombinePass());

    for (auto& F : *module) {
        if (!F.isDeclaration()) {
            earlyFPM.run(F, FAM);
        }
    }

    if (level >= OptimizationLevel::O1) {
        midFPM.addPass(llvm::DCEPass());
        midFPM.addPass(llvm::SimplifyCFGPass());

        for (auto& F : *module) {
            if (!F.isDeclaration()) {
                midFPM.run(F, FAM);
            }
        }
    }

    if (level >= OptimizationLevel::O2) {
        llvm::FunctionPassManager o2FPM;
        o2FPM.addPass(llvm::ReassociatePass());
        o2FPM.addPass(llvm::GVNPass());
        o2FPM.addPass(llvm::SCCPPass());
        o2FPM.addPass(llvm::ADCEPass());
        o2FPM.addPass(llvm::SimplifyCFGPass());

        for (auto& F : *module) {
            if (!F.isDeclaration()) {
                o2FPM.run(F, FAM);
            }
        }

        MPM.addPass(llvm::GlobalOptPass());
        MPM.addPass(llvm::createModuleToFunctionPassAdaptor(llvm::InstCombinePass()));
    }

    if (level >= OptimizationLevel::O3) {
        llvm::LoopPassManager LPM;
        LPM.addPass(llvm::LoopRotatePass());
        LPM.addPass(llvm::LoopDeletionPass());
        LPM.addPass(llvm::IndVarSimplifyPass());

        llvm::FunctionPassManager loopFPM;
        loopFPM.addPass(llvm::createFunctionToLoopPassAdaptor(std::move(LPM)));

        for (auto& F : *module) {
            if (!F.isDeclaration()) {
                loopFPM.run(F, FAM);
            }
        }

        llvm::FunctionPassManager vecFPM;
        vecFPM.addPass(llvm::LoopVectorizePass());
        vecFPM.addPass(llvm::SLPVectorizerPass());

        for (auto& F : *module) {
            if (!F.isDeclaration()) {
                vecFPM.run(F, FAM);
            }
        }

        llvm::FunctionPassManager cleanupFPM;
        cleanupFPM.addPass(llvm::InstCombinePass());
        cleanupFPM.addPass(llvm::SimplifyCFGPass());

        for (auto& F : *module) {
            if (!F.isDeclaration()) {
                cleanupFPM.run(F, FAM);
            }
        }
    }

    if (level == OptimizationLevel::Os || level == OptimizationLevel::Oz) {
        llvm::FunctionPassManager sizeFPM;
        sizeFPM.addPass(llvm::SimplifyCFGPass());
        sizeFPM.addPass(llvm::DCEPass());

        for (auto& F : *module) {
            if (!F.isDeclaration()) {
                sizeFPM.run(F, FAM);
            }
        }
    }

    lateFPM.addPass(llvm::InstCombinePass());
    lateFPM.addPass(llvm::SimplifyCFGPass());
    lateFPM.addPass(llvm::DCEPass());

    for (auto& F : *module) {
        if (!F.isDeclaration()) {
            lateFPM.run(F, FAM);
        }
    }

    if (!MPM.isEmpty()) {
        MPM.run(*module, MAM);
    }

    if (!verifyModule(module, false)) {
        std::cerr << "[Optimizer] Warning: IR verification failed after optimization" << std::endl;
    }
}

bool Optimizer::verifyModule(llvm::Module* module, bool fatal) {
    std::string errorMsg;
    llvm::raw_string_ostream os(errorMsg);

    if (llvm::verifyModule(*module, &os)) {
        std::cerr << "[Optimizer] IR Verification Failed:" << std::endl;
        std::cerr << errorMsg << std::endl;

        if (fatal) {
            std::cerr << "[Optimizer] Fatal error: Invalid IR, aborting" << std::endl;
            exit(1);
        }

        return false;
    }

    return true;
}

void Optimizer::printOptimizationStats(llvm::Module* module) {
    (void)module;
}

} // namespace xypher

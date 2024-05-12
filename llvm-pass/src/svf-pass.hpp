#ifndef SVF_PASS_HPP
#define SVF_PASS_HPP

#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassPlugin.h>

#include <string>

namespace svfpass
{

/**
 * @brief :class:`SVFPass` defines an LLVM pass that can be run on an LLVM
 * module as part of the compilation pass pipeline; this just performs an
 * example dummy analysis
 */
class SVFPass : public llvm::PassInfoMixin<SVFPass>
{
  public:
    /** Define this pass as required so it is always executed, even with -O0 */
    static bool isRequired() { return true; }

    /** Sets the name of the pass to call it with 'opt -passes=...' */
    virtual std::string getPassName() const { return "svf-pass"; }

    /** Default constructor */
    SVFPass() {}

    /**
     * @brief Define the main entry-point when executed by the LLVM pass manager
     *
     * @param mod the LLVM module object the pass is currently running on
     * @param mod_mgr the LLVM module analysis manager object for current
     * pipeline
     * @return llvm::PreservedAnalyses::all() if nothing changed; else ::none()
     */
    llvm::PreservedAnalyses run(llvm::Module &mod, llvm::ModuleAnalysisManager &mod_mgr);
};

}    // namespace svfpass

/**
 * @brief Defines the access point of the LLVM pass; enables `opt -passes=...`
 *
 * @return llvm::PassPluginLibraryInfo
 */
llvm::PassPluginLibraryInfo getSVFPassPluginInfo();

/**
 * @brief llvmGetPassPluginInfo() gets called to resolve which pass is being run
 *
 * @return ::llvm::PassPluginLibraryInfo
 */
extern "C" LLVM_ATTRIBUTE_WEAK llvm::PassPluginLibraryInfo llvmGetPassPluginInfo();

#endif

#include "svf-pass.hpp"

#include <Graphs/IRGraph.h>
#include <Graphs/SVFG.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Config/llvm-config.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/Compiler.h>
#include <llvm/Support/PrettyStackTrace.h>
#include <llvm/Support/raw_ostream.h>
#include <SVF/MSSA/SVFGBuilder.h>
#include <SVF/SVF-LLVM/LLVMModule.h>
#include <SVF/SVF-LLVM/LLVMUtil.h>
#include <SVF/SVF-LLVM/SVFIRBuilder.h>
#include <SVF/SVFIR/SVFIR.h>
#include <SVF/SVFIR/SVFModule.h>
#include <SVF/Util/SVFUtil.h>
#include <SVF/WPA/Andersen.h>
#include <SVFIR/SVFValue.h>
#include <Util/GeneralType.h>
#include <Util/WorkList.h>

namespace svfpass
{

// Could obviously be written much more concise; this is just to showcase the problem points
static const SVF::SVFGNode *getDefSite(SVF::PAG *pag, SVF::SVFG *svfg, llvm::Value *val)
{
    assert(pag && svfg && "Cannot get node with nullptr pag/svfg!");
    if (!val) return nullptr;

    // Get SVF value for LLVM value; not all LLVM values have a valid SVF value
    SVF::SVFValue *svf_val = SVF::LLVMModuleSet::getLLVMModuleSet()->getSVFValue(val);
    if (!svf_val) return nullptr;

    // Get the node (ID) in the Program Assignment Graph (PAG); not all SVF values do
    if (!pag->hasValueNode(svf_val)) return nullptr;
    SVF::NodeID node_id = pag->getValueNode(svf_val);

    // Get the actual node of this ID; not all IDs have valid nodes
    if (!pag->hasGNode(node_id)) return nullptr;
    SVF::PAGNode *pag_node = pag->getGNode(node_id);
    if (!pag_node) return nullptr;

    // Lastly, get the definition node of this PAG node in the SVFG IFF one exists
    if (!svfg->hasDefSVFGNode(pag_node)) return nullptr;
    const SVF::SVFGNode *svfg_node = svfg->getDefSVFGNode(pag_node);
    if (!svfg_node) return nullptr;
    return svfg_node;
}

SVF::Set<const SVF::SVFGNode *> walkSVFG(const SVF::SVFGNode *node)
{
    SVF::Set<const SVF::SVFGNode *>          visited;
    SVF::FIFOWorkList<const SVF::SVFGNode *> worklist;

    // Lambda for adding to worklist iff the node hasn't been visited before
    auto addWork = [&](const SVF::SVFGNode *_node) -> void {
        if (_node && visited.insert(_node).second) worklist.push(_node);
    };

    // Start with initial node
    addWork(node);

    // Explore while worklist is not empty; always add all incoming edges to worklist
    while (!worklist.empty()) {
        const SVF::SVFGNode *hd = worklist.pop();
        for (const SVF::SVFGEdge *ie : hd->getInEdges()) addWork(ie->getSrcNode());
    }

    // Return all nodes visited during exploration
    return visited;
}

/**
 * @brief Define the main entry-point when executed by the LLVM pass manager
 *
 * @param mod the LLVM module object the pass is currently running on
 * @param mod_mgr the LLVM module analysis manager object for current pipeline
 * @return llvm::PreservedAnalyses::all() if nothing changed; else ::none()
 */
llvm::PreservedAnalyses SVFPass::run(llvm::Module &mod, llvm::ModuleAnalysisManager &mod_mgr)
{
    llvm::errs() << "Running SVFPass on module: " << mod.getName() << "\n";

    // This is required; otherwise parsing extapi.bc will fail
    llvm::EnablePrettyStackTrace();
    mod.getContext().setDiscardValueNames(false);

    SVF::LLVMModuleSet *mod_set = SVF::LLVMModuleSet::getLLVMModuleSet();
    SVF::SVFModule     *svf_mod = SVF::LLVMModuleSet::buildSVFModule(mod);
    assert(mod_set && svf_mod && "Failed to get svf module/module set");

    SVF::SVFIRBuilder *ir_builder   = new SVF::SVFIRBuilder(svf_mod);
    SVF::SVFGBuilder  *svfg_builder = new SVF::SVFGBuilder(true);
    assert(ir_builder && svfg_builder && "Failed to create IR/SVFG builders");

    SVF::PAG              *pag  = ir_builder->build();
    SVF::AndersenWaveDiff *pta  = SVF::AndersenWaveDiff::createAndersenWaveDiff(pag);
    SVF::SVFG             *svfg = svfg_builder->buildFullSVFG(pta);
    assert(pag && pta && svfg && "Failed to get pag/pta/svfg!");

    // Just an example, but for each call, walk the SVFG
    // backwards to find where its arguments were defined
    for (llvm::Function &func : mod.functions()) {
        for (llvm::Instruction &inst : llvm::instructions(func)) {
            llvm::CallBase *caller = llvm::dyn_cast<llvm::CallBase>(&inst);
            llvm::Function *callee = caller ? caller->getCalledFunction() : nullptr;
            if (!caller || !callee) continue;    // Skip unknowns for this example

            llvm::outs() << "Found call to: " << llvm::demangle(callee->getName().str()) << "\n";

            for (llvm::Value *arg : caller->args()) {
                llvm::outs() << "Finding definition sites for argument: " << *arg << "\n";
                for (const SVF::SVFGNode *def : walkSVFG(getDefSite(pag, svfg, arg)))
                    llvm::outs() << "Definition site: " << def->toString() << "\n";
            }
        }
    }

    return llvm::PreservedAnalyses::all();
}

}    // namespace svfpass

/**
 * @brief Defines the access point of the LLVM pass; enables `opt -passes=...`
 *
 * @return llvm::PassPluginLibraryInfo
 */
llvm::PassPluginLibraryInfo getSVFPassPluginInfo()
{
    return {LLVM_PLUGIN_API_VERSION, "svf-pass", LLVM_VERSION_STRING, [](llvm::PassBuilder &builder) {
                // Adds SVFPass to the front of the compiler pipeline
                builder.registerPipelineStartEPCallback(
                    [](llvm::ModulePassManager &mgr, llvm::OptimizationLevel) { mgr.addPass(svfpass::SVFPass()); });

                // Adds SVFPass to the end of the default LTO pipeline
                builder.registerFullLinkTimeOptimizationLastEPCallback(
                    [](llvm::ModulePassManager &mgr, llvm::OptimizationLevel) { mgr.addPass(svfpass::SVFPass()); });

                // If called through `-opt -passes=...`; add to pipeline
                builder.registerPipelineParsingCallback([](llvm::StringRef          name,
                                                           llvm::ModulePassManager &mgr,
                                                           llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                    if (name.equals_insensitive("svf-pass")) {
                        mgr.addPass(svfpass::SVFPass());
                        return true;
                    }
                    return false;
                });
            }};
}

/**
 * @brief llvmGetPassPluginInfo() gets called to resolve which pass is being run
 *
 * @return ::llvm::PassPluginLibraryInfo
 */
extern "C" LLVM_ATTRIBUTE_WEAK llvm::PassPluginLibraryInfo llvmGetPassPluginInfo()
{
    return getSVFPassPluginInfo();
}

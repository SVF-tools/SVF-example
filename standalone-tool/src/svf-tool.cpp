//===- svf-ex.cpp -- A driver example of
// SVF-------------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013->  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===-----------------------------------------------------------------------===//

/*
 // A driver program of SVF including usages of SVF APIs
 //
 // Author: Yulei Sui,
 // Modified by: Johannes Blaser
 */

#include <llvm/Support/raw_ostream.h>
#include <SVF/Graphs/SVFG.h>
#include <SVF/SVF-LLVM/LLVMModule.h>
#include <SVF/SVF-LLVM/SVFIRBuilder.h>
#include <SVF/Util/Options.h>
#include <SVF/WPA/Andersen.h>

/*!
 * An example to query alias results of two LLVM values
 */
SVF::AliasResult aliasQuery(SVF::PointerAnalysis *pta, llvm::Value *v1, llvm::Value *v2)
{
    SVF::SVFValue *val1 = SVF::LLVMModuleSet::getLLVMModuleSet()->getSVFValue(v1);
    SVF::SVFValue *val2 = SVF::LLVMModuleSet::getLLVMModuleSet()->getSVFValue(v2);
    return pta->alias(val1, val2);
}

/*!
 * An example to print points-to set of an LLVM value
 */
std::string printPts(SVF::PointerAnalysis *pta, llvm::Value *val)
{
    std::string              str;
    llvm::raw_string_ostream rawstr(str);
    SVF::SVFValue           *svfval  = SVF::LLVMModuleSet::getLLVMModuleSet()->getSVFValue(val);

    SVF::NodeID              pNodeId = pta->getPAG()->getValueNode(svfval);
    const SVF::PointsTo     &pts     = pta->getPts(pNodeId);
    for (SVF::PointsTo::iterator ii = pts.begin(), ie = pts.end(); ii != ie; ii++) {
        rawstr << " " << *ii << " ";
        SVF::PAGNode *targetObj = pta->getPAG()->getGNode(*ii);
        if (targetObj->hasValue()) rawstr << "(" << targetObj->getValue()->toString() << ")\t ";
    }

    return rawstr.str();
}

/*!
 * An example to query/collect all successor nodes from a ICFGNode (iNode)
 along
 * control-flow graph (ICFG)
 */
void traverseOnICFG(SVF::ICFG *icfg, const llvm::Instruction *inst)
{
    SVF::SVFInstruction                     *svfinst = SVF::LLVMModuleSet::getLLVMModuleSet()->getSVFInstruction(inst);

    SVF::ICFGNode                           *iNode   = icfg->getICFGNode(svfinst);
    SVF::FIFOWorkList<const SVF::ICFGNode *> worklist;
    SVF::Set<const SVF::ICFGNode *>          visited;
    worklist.push(iNode);

    /// Traverse along VFG
    while (!worklist.empty()) {
        const SVF::ICFGNode *iNode = worklist.pop();
        for (const SVF::ICFGEdge *edge : iNode->getOutEdges()) {
            SVF::ICFGNode *succNode = edge->getDstNode();
            if (visited.find(succNode) == visited.end()) {
                visited.insert(succNode);
                worklist.push(succNode);
            }
        }
    }
}

/*!
 * An example to query/collect all the uses of a definition of a value along
 * value-flow graph (VFG)
 */
void traverseOnVFG(const SVF::SVFG *vfg, llvm::Value *val)
{
    SVF::SVFIR                             *pag    = SVF::SVFIR::getPAG();
    SVF::SVFValue                          *svfval = SVF::LLVMModuleSet::getLLVMModuleSet()->getSVFValue(val);

    SVF::PAGNode                           *pNode  = pag->getGNode(pag->getValueNode(svfval));
    const SVF::VFGNode                     *vNode  = vfg->getDefSVFGNode(pNode);
    SVF::FIFOWorkList<const SVF::VFGNode *> worklist;
    SVF::Set<const SVF::VFGNode *>          visited;
    worklist.push(vNode);

    /// Traverse along VFG
    while (!worklist.empty()) {
        const SVF::VFGNode *vNode = worklist.pop();
        for (const SVF::SVFGEdge *edge : vNode->getOutEdges()) {
            SVF::VFGNode *succNode = edge->getDstNode();
            if (visited.find(succNode) == visited.end()) {
                visited.insert(succNode);
                worklist.push(succNode);
            }
        }
    }

    /// Collect all LLVM Values
    for (const SVF::SVFGNode *node : visited) {
        if (const SVF::SVFValue *svf_val = node->getValue()) {
            if (const llvm::Value *llvm_val = SVF::LLVMModuleSet::getLLVMModuleSet()->getLLVMValue(svf_val))
                llvm::outs() << "LLVM value: " << *llvm_val << "\n";
        }
    }
}

int main(int argc, char **argv, char **envp)
{
    std::vector<std::string> moduleNameVec;
    moduleNameVec
        = OptionBase::parseOptions(argc, argv, "Whole Program Points-to Analysis", "[options] <input-bitcode...>");

    if (SVF::Options::WriteAnder() == "ir_annotator") SVF::LLVMModuleSet::preProcessBCs(moduleNameVec);

    SVF::SVFModule *svfModule = SVF::LLVMModuleSet::buildSVFModule(moduleNameVec);

    /// Build Program Assignment Graph (SVFIR)
    SVF::SVFIRBuilder builder(svfModule);
    SVF::SVFIR       *pag = builder.build();

    /// Create Andersen's pointer analysis
    SVF::Andersen *ander = SVF::AndersenWaveDiff::createAndersenWaveDiff(pag);

    /// Query aliases
    /// aliasQuery(ander,value1,value2);

    /// Print points-to information
    /// printPts(ander, value1);

    /// Call Graph
    SVF::PTACallGraph *callgraph = ander->getPTACallGraph();

    /// ICFG
    SVF::ICFG *icfg = pag->getICFG();
    (void)icfg;

    /// Value-Flow Graph (VFG)
    SVF::VFG *vfg = new SVF::VFG(callgraph);

    /// Sparse value-flow graph (SVFG)
    SVF::SVFGBuilder svfBuilder;
    SVF::SVFG       *svfg = svfBuilder.buildFullSVFG(ander);
    (void)svfg;

    /// Collect uses of an LLVM Value
    /// traverseOnVFG(svfg, value);

    /// Collect all successor nodes on ICFG
    /// traverseOnICFG(icfg, value);

    // clean up memory
    delete vfg;
    SVF::AndersenWaveDiff::releaseAndersenWaveDiff();
    SVF::SVFIR::releaseSVFIR();

    SVF::LLVMModuleSet::getLLVMModuleSet()->dumpModulesToFile(".svf.bc");
    SVF::LLVMModuleSet::releaseLLVMModuleSet();

    llvm::llvm_shutdown();
    return 0;
}

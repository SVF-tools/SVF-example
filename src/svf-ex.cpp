//===- svf-ex.cpp -- A driver example of SVF-------------------------------------//
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
 */

#include "SVF-FE/LLVMUtil.h"
#include "Graphs/SVFG.h"
#include "WPA/Andersen.h"
#include "SABER/LeakChecker.h"
#include "SVF-FE/PAGBuilder.h"
#include "Andersen_pta.h"

using namespace SVF;
using namespace llvm;
using namespace std;

static llvm::cl::opt<std::string> InputFilename(cl::Positional,
        llvm::cl::desc("<input bitcode>"), llvm::cl::init("-"));

static llvm::cl::opt<bool> LEAKCHECKER("leak", llvm::cl::init(false),
                                       llvm::cl::desc("Memory Leak Detection"));

ICFGNode *srcNode = NULL;
ICFGNode *sinkNode = NULL;
PAG *pag;


/*!
 * An example to query alias results of two LLVM values
 */
AliasResult aliasQuery(PointerAnalysis* pta, Value* v1, Value* v2){
	return pta->alias(v1,v2);
}

/*!
 * An example to print points-to set of an LLVM value
 */
std::string printPts(PointerAnalysis* pta, Value* val){

    std::string str;
    raw_string_ostream rawstr(str);

    NodeID pNodeId = pta->getPAG()->getValueNode(val);
    const NodeBS& pts = pta->getPts(pNodeId);
    for (NodeBS::iterator ii = pts.begin(), ie = pts.end();
            ii != ie; ii++) {
        rawstr << " " << *ii << " ";
        PAGNode* targetObj = pta->getPAG()->getPAGNode(*ii);
        if(targetObj->hasValue()){
            rawstr << "(" <<*targetObj->getValue() << ")\t ";
        }
    }

    return rawstr.str();

}


/*!
 * An example to query/collect all successor nodes from a ICFGNode (iNode) along control-flow graph (ICFG)
 */
void traverseOnICFG(ICFG* icfg, const Instruction* inst){
	ICFGNode* iNode = icfg->getBlockICFGNode(inst);
	FIFOWorkList<const ICFGNode*> worklist;
	std::set<const ICFGNode*> visited;
	worklist.push(iNode);

	/// Traverse along VFG
	while (!worklist.empty()) {
		const ICFGNode* vNode = worklist.pop();
		for (ICFGNode::const_iterator it = iNode->OutEdgeBegin(), eit =
				iNode->OutEdgeEnd(); it != eit; ++it) {
			ICFGEdge* edge = *it;
			ICFGNode* succNode = edge->getDstNode();
			if (visited.find(succNode) == visited.end()) {
				visited.insert(succNode);
				worklist.push(succNode);
			}
		}
	}
}

/*!
 * An example to query/collect all the uses of a definition of a value along value-flow graph (VFG)
 */
void traverseOnVFG(const SVFG* vfg, Value* val){
	PAG* pag = PAG::getPAG();

    PAGNode* pNode = pag->getPAGNode(pag->getValueNode(val));
    const VFGNode* vNode = vfg->getDefSVFGNode(pNode);
    FIFOWorkList<const VFGNode*> worklist;
    std::set<const VFGNode*> visited;
    worklist.push(vNode);

	/// Traverse along VFG
	while (!worklist.empty()) {
		const VFGNode* vNode = worklist.pop();
		for (VFGNode::const_iterator it = vNode->OutEdgeBegin(), eit =
				vNode->OutEdgeEnd(); it != eit; ++it) {
			VFGEdge* edge = *it;
			VFGNode* succNode = edge->getDstNode();
			if (visited.find(succNode) == visited.end()) {
				visited.insert(succNode);
				worklist.push(succNode);
			}
		}
	}

    /// Collect all LLVM Values
    for(std::set<const VFGNode*>::const_iterator it = visited.begin(), eit = visited.end(); it!=eit; ++it){
    	const VFGNode* node = *it;
    	/// can only query VFGNode involving top-level pointers (starting with % or @ in LLVM IR)
    	/// PAGNode* pNode = vfg->getLHSTopLevPtr(node);
    	/// Value* val = pNode->getValue();
    }
}

//traverse from src node to sink node give the all paths
void dfsOnICFG(ICFGNode* src,ICFGNode* snk, vector<const ICFGNode*>& seq, set<const ICFGNode*>& visited)
{
    visited.insert(src);
    seq.push_back(src);
    if (src->getId()==snk->getId())
    {
        //output the sequence
        for (const ICFGNode* & eit : seq){
            cout << "-->" << eit->getId() ;
        }
        cout << "\n";
    }
    for ( auto it = src->OutEdgeBegin(); it != src->OutEdgeEnd(); it ++) {
        ICFGNode *cur = (*it)->getDstNode();
        if (visited.find(cur) == visited.end()) {
            dfsOnICFG(cur, snk, seq,visited);
        }
    }
    visited.erase(src);
    seq.pop_back();
}

void findSrcSinkPaths(ICFG *icfg){
    ICFGNode *srcNode,*sinkNode;
    srcNode = NULL;
    sinkNode = NULL;
    for(ICFG::iterator it = icfg->begin(), eit = icfg->end(); it!=eit; ++it)
    {
        ICFGNode* node = it->second;
        // check if the call node's callee is src or sink
        if (CallBlockNode* call = SVFUtil::dyn_cast<CallBlockNode>(node)){
            const SVFFunction* fun = SVFUtil::getCallee(call->getCallSite());
            if(fun->getName().equals("src")){
                srcNode = node;
            }
            else if(fun->getName().equals("sink"))
                sinkNode = node;
        }
    }
    if( srcNode!= NULL && sinkNode != NULL){
        outs() << "ICFG Source Node and Sink Node found: \n" << "srcID:"<<
        srcNode->getId()<< "\tsinkID:"<< sinkNode->getId() << "\n" << "Path(s)：\n";
        //mark for the status of node in seq
        set<const ICFGNode *> visited;
        //store for node sequence
        vector<const ICFGNode *> seq;
        dfsOnICFG(srcNode, sinkNode,seq,visited);
    }else {
        outs() << "src or sink not found! \n";
    }
}

int main(int argc, char ** argv) {

    int arg_num = 0;
    char **arg_value = new char*[argc];
    std::vector<std::string> moduleNameVec;
    SVFUtil::processArguments(argc, argv, arg_num, arg_value, moduleNameVec);
    cl::ParseCommandLineOptions(arg_num, arg_value,
                                "Whole Program Points-to Analysis\n");

    SVFModule* svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);

    /// Build Program Assignment Graph (PAG)
    PAGBuilder builder;
    pag = builder.build (svfModule);
    pag->dump ("pag");

    /// Create Andersen's pointer analysis
    Andersen *ander = AndersenWaveDiff::createAndersenWaveDiff (pag);

    /// Query aliases
    /// aliasQuery(ander,value1,value2);

    /// Print points-to information
    /// printPts(ander, value1);

    /// Call Graph
    PTACallGraph *callgraph = ander->getPTACallGraph ();
    callgraph->dump ("callgraph");

    /// ICFG
    ICFG *icfg = pag->getICFG ();
    icfg->dump ("icfg");
	// find all paths from source node to sink node
	findSrcSinkPaths(icfg);

    /// Value-Flow Graph (VFG)
    VFG *vfg = new VFG (callgraph);
    vfg->dump ("vfg");

    /// Sparse value-flow graph (SVFG)
    SVFGBuilder svfBuilder;
    SVFG *svfg = svfBuilder.buildFullSVFGWithoutOPT (ander);
    svfg->dump ("svfg");

    /// Collect uses of an LLVM Value
    /// traverseOnVFG(svfg, value);

    /// Collect all successor nodes on ICFG
    /// traverseOnICFG(icfg, value);

    LeakChecker *saber = new LeakChecker (); // if no checker is specified, we use leak checker as the default one.
    saber->runOnModule (svfModule);
	Andersen_pta * andersen_pta = new Andersen_pta(pag);
    andersen_pta -> pointToAnalysis();
    return 0;
}

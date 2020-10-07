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

using namespace SVF;
using namespace llvm;
using namespace std;

static llvm::cl::opt<std::string> InputFilename(cl::Positional,
        llvm::cl::desc("<input bitcode>"), llvm::cl::init("-"));

static llvm::cl::opt<bool> LEAKCHECKER("leak", llvm::cl::init(false),
                                       llvm::cl::desc("Memory Leak Detection"));

ICFGNode *srcNode = NULL;
ICFGNode *sinkNode = NULL;
typedef PTData<NodeID,PointsTo> PTDataTy;
typedef DiffPTData<NodeID,PointsTo,EdgeID> DiffPTDataTy;
ConstraintGraph * consCG;
FIFOWorkList<NodeID> pta_worklist;
PTDataTy* ptD;
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
    NodeBS& pts = pta->getPts(pNodeId);
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
    for(ICFG::iterator it = icfg->begin(), eit = icfg->end(); it!=eit; ++it)
    {
        ICFGNode* node = it->second;
        // check if the call node's callee is src or sink
        if (CallBlockNode* call = SVFUtil::dyn_cast<CallBlockNode>(node)){
            const SVFFunction* fun = SVFUtil::getCallee(call->getCallSite());
            if(fun->getName().equals("src")){
                srcNode = node;}
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

NodeID sccRepNode(NodeID id){
    return consCG->sccRepNode(id);
}

bool addPts(NodeID id, NodeID ptd){
    return ptD->addPts(id,ptd);
}

PointsTo& getPts(NodeID id){
    id = sccRepNode(id);
    return ptD -> getPts(id);
}

bool unionPts(NodeID id, const PointsTo& target){
    id = sccRepNode(id);
    return ptD->unionPts(id, target);
}

//Process copy edges: src --copy--> dst, union pts(dst) with pts(src)
bool processCopy(NodeID node, const ConstraintEdge* edge)
{
    assert((SVFUtil::isa<CopyCGEdge>(edge)) && "not copy/call/ret ??");
    PointsTo& srcPts = getPts(node);
    NodeID dst = edge->getDstID();
    bool changed = unionPts(dst, srcPts);
    if (changed)
        pta_worklist.push(sccRepNode(dst));
    return changed;
}

void setObjFieldInsensitive(NodeID id)
{
    MemObj* mem =  const_cast<MemObj*>(pag->getBaseObj(id));
    mem->setFieldInsensitive();
}

bool isFieldInsensitive(NodeID id)
{
    const MemObj* mem =  pag->getBaseObj(id);
    return mem->isFieldInsensitive();
}

void addTypeForGepObjNode(NodeID, const NormalGepCGEdge*)
{
    return;
}

bool matchType(NodeID, NodeID, const NormalGepCGEdge*)
{
    return true;
}

bool processGepPts(PointsTo& pts, const GepCGEdge* edge)
{
    PointsTo tmpDstPts;
    for (PointsTo::iterator piter = pts.begin(), epiter = pts.end(); piter != epiter; ++piter)
    {
        /// get the object
        NodeID ptd = *piter;
        /// handle blackhole and constant
        if (consCG->isBlkObjOrConstantObj(ptd))
        {
            tmpDstPts.set(*piter);
        }
        else
        {
            /// handle variant gep edge
            /// If a pointer connected by a variant gep edge,
            /// then set this memory object to be field insensitive
            if (SVFUtil::isa<VariantGepCGEdge>(edge))
            {
                if (isFieldInsensitive(ptd) == false)
                {
                    setObjFieldInsensitive(ptd);
                    consCG->addNodeToBeCollapsed(consCG->getBaseObjNode(ptd));
                }
                // add the field-insensitive node into pts.
                NodeID baseId = consCG->getFIObjNode(ptd);
                tmpDstPts.set(baseId);
            }
            /// Otherwise process invariant (normal) gep
            // TODO: after the node is set to field insensitive, handling invaraint gep edge may lose precision
            // because offset here are ignored, and it always return the base obj
            else if (const NormalGepCGEdge* normalGepEdge = SVFUtil::dyn_cast<NormalGepCGEdge>(edge))
            {
                if (!matchType(edge->getSrcID(), ptd, normalGepEdge))
                    continue;
                NodeID fieldSrcPtdNode = consCG->getGepObjNode(ptd,	normalGepEdge->getLocationSet());
                tmpDstPts.set(fieldSrcPtdNode);
                addTypeForGepObjNode(fieldSrcPtdNode, normalGepEdge);
            }
            else
            {
                assert(false && "new gep edge?");
            }
        }
    }

    NodeID dstId = edge->getDstID();
    if (unionPts(dstId, tmpDstPts))
    {
        pta_worklist.push(sccRepNode(dstId));
        return true;
    }
    return false;
}

bool processGep(NodeID node, const GepCGEdge* edge){
    PointsTo& srcPts = getPts(node);
    return processGepPts(srcPts, edge);
}

bool isNonPointerObj(NodeID ptd)
{
    return pag->isNonPointerObj(ptd);
}

/// Handle propagated points-to set.
void updatePropaPts(NodeID dstId, NodeID srcId){
    NodeID srcRep = sccRepNode(srcId);
    NodeID dstRep = sccRepNode(dstId);
    SVFUtil::cast<DiffPTDataTy>(ptD)->updatePropaPtsMap(srcRep, dstRep);
}

/// Add copy edge on constraint graph
bool addCopyEdge(NodeID src, NodeID dst){
    if (consCG->addCopyCGEdge(src, dst))
    {
        updatePropaPts(src, dst);
        return true;
    }
    return false;
}

//Process load edges: src --load--> dst, node in pts(src) ==>  node--copy-->dst
bool processLoad(NodeID node, const ConstraintEdge* load){
    if (pag->isConstantObj(node) || isNonPointerObj(node))
        return false;
    NodeID dst = load->getDstID();
    return addCopyEdge(node, dst);
}

//Process store edges: node in pts(dst) ==>  src--copy-->node
bool processStore(NodeID node, const ConstraintEdge* store){
    if (pag->isConstantObj(node) || isNonPointerObj(node))
        return false;
    NodeID src = store->getSrcID();
    return addCopyEdge(src, node);
}

void processAddr(const AddrCGEdge* addr)
{
    NodeID dst = addr->getDstID();
    NodeID src = addr->getSrcID();
    if(addPts(dst,src))
        pta_worklist.push(sccRepNode(dst));
}

//init and add all addr nodes to the worklist
void processAllAddr(){
    for (ConstraintGraph::const_iterator nodeIt = consCG->begin(), nodeEit = consCG->end(); nodeIt != nodeEit; nodeIt++)
    {
        ConstraintNode * cgNode = nodeIt->second;
        for (ConstraintNode::const_iterator it = cgNode->incomingAddrsBegin(), eit = cgNode->incomingAddrsEnd();
                it != eit; ++it)
            processAddr(SVFUtil::cast<AddrCGEdge>(*it));
    }
}

void handleCopyGep(ConstraintNode* node){
    NodeID nodeId = node->getId();
    for (ConstraintEdge* edge : node->getCopyOutEdges()){
        processCopy(nodeId, edge);
    }
    for (ConstraintEdge* edge : node->getGepOutEdges())
    {
        if (GepCGEdge* gepEdge = SVFUtil::dyn_cast<GepCGEdge>(edge))
            processGep(nodeId, gepEdge);
    }
}

void handleLoadStore(ConstraintNode *node){
    NodeID nodeId = node->getId();
    for (PointsTo::iterator piter = getPts(nodeId).begin(), epiter = getPts(nodeId).end(); piter != epiter; ++piter)
    {
        NodeID ptd = *piter;
        // handle load
        for (ConstraintNode::const_iterator it = node->outgoingLoadsBegin(), eit = node->outgoingLoadsEnd(); it != eit; ++it)
        {
            if (processLoad(ptd, *it))
                pta_worklist.push(sccRepNode(ptd));
        }

        // handle store
        for (ConstraintNode::const_iterator it = node->incomingStoresBegin(), eit = node->incomingStoresEnd(); it != eit; ++it)
        {
            if (processStore(ptd, *it))
                pta_worklist.push(sccRepNode((*it)->getSrcID()));
        }
    }
}

void pointToAnalysis(){
    consCG = new ConstraintGraph(pag);
    outs()<< "consCG initial\n";
    consCG->dump("consCG_initial");
    // Initialize worklist
    processAllAddr();
    while (!pta_worklist.empty()){
        NodeID nodeId = sccRepNode(pta_worklist.pop());
        // sub nodes do not need to be processed
        if (sccRepNode(nodeId) != nodeId)
            return;
        ConstraintNode* node = consCG->getConstraintNode(nodeId);
        handleLoadStore(node);
        handleCopyGep(node);
    }
    outs()<< "consCG finalize\n";
    consCG->dump("consCG_final");
    consCG->print();
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
	pointToAnalysis();
    return 0;
}

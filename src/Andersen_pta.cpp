#include "Andersen_pta.h"

using namespace SVF;
using namespace SVFUtil;
using namespace llvm;
using namespace std;

void Andersen_pta::processAddr(const AddrCGEdge* addr){
    NodeID dst = addr->getDstID();
    NodeID src = addr->getSrcID();
    if(addPts(dst,src))
        pushIntoWorklist(dst);
}

void Andersen_pta::processAllAddr(){
    for (ConstraintGraph::const_iterator nodeIt = consCG->begin(), nodeEit = consCG->end(); nodeIt != nodeEit; nodeIt++)
    {
        ConstraintNode * cgNode = nodeIt->second;
        for (ConstraintNode::const_iterator it = cgNode->incomingAddrsBegin(), eit = cgNode->incomingAddrsEnd();
                it != eit; ++it)
            processAddr(SVFUtil::cast<AddrCGEdge>(*it));
    }
}

bool Andersen_pta::processStore(NodeID node, const ConstraintEdge* store){
    if (pag->isConstantObj(node) || isNonPointerObj(node))
        return false;
    NodeID src = store->getSrcID();
    return addCopyEdge(src, node);
}
        
bool Andersen_pta::processLoad(NodeID node, const ConstraintEdge* load){
    if (pag->isConstantObj(node) || isNonPointerObj(node))
        return false;
    NodeID dst = load->getDstID();
    return addCopyEdge(node, dst);
}

bool Andersen_pta::processCopy(NodeID node, const ConstraintEdge* edge){
    assert((SVFUtil::isa<CopyCGEdge>(edge)) && "not copy/call/ret ??");
    const PointsTo& srcPts = getPts(node);
    NodeID dst = edge->getDstID();
    bool changed = unionPts(dst, srcPts);
    if (changed)
       pushIntoWorklist(dst);
    return changed;
}

bool Andersen_pta::processGepPts(const PointsTo& pts, const GepCGEdge* edge){
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
            /// handle variant gep edge. If a pointer connected by a variant gep edge, then set this memory object to be field insensitive
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
        pushIntoWorklist(dstId);
        return true;
    }
    return false;
}

bool Andersen_pta::processGep(NodeID node, const GepCGEdge* edge){
    const PointsTo& srcPts = getPts(node);
    return processGepPts(srcPts, edge);
}

void Andersen_pta::handleCopyGep(ConstraintNode* node){
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

void Andersen_pta::handleLoadStore(ConstraintNode *node){
    NodeID nodeId = node->getId();
    for (PointsTo::iterator piter = getPts(nodeId).begin(), epiter = getPts(nodeId).end(); piter != epiter; ++piter)
    {
        NodeID ptd = *piter;
        // handle load
        for (ConstraintNode::const_iterator it = node->outgoingLoadsBegin(), eit = node->outgoingLoadsEnd(); it != eit; ++it)
        {
            if (processLoad(ptd, *it))
                pushIntoWorklist(ptd);
        }

        // handle store
        for (ConstraintNode::const_iterator it = node->incomingStoresBegin(), eit = node->incomingStoresEnd(); it != eit; ++it)
        {
            if (processStore(ptd, *it))
                pushIntoWorklist((*it)->getSrcID());
        }
    }
}

void Andersen_pta::pointToAnalysis(){
    consCG = new ConstraintGraph(pag);
    consCG->dump("consCG_initial");
    // Initialize worklist
    processAllAddr();
    while (!isWorklistEmpty()){
        NodeID nodeId = popFromWorklist();
        // sub nodes do not need to be processed
        if (sccRepNode(nodeId) != nodeId)
            return;
        ConstraintNode* node = consCG->getConstraintNode(nodeId);
        handleLoadStore(node);
        handleCopyGep(node);
    }
    consCG->dump("consCG_final");
    consCG->print();
}
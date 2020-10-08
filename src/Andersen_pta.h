#include "WPA/Andersen.h"

namespace SVF
{
    class Andersen_pta : public Andersen{

        public: 
        /// Constructor
        Andersen_pta(PAG* pag) : Andersen(pag){

        }

        /// Destructor
        virtual ~Andersen_pta(){

        }

        void processAddr(const AddrCGEdge* addr);

        void processAllAddr();

        bool processStore(NodeID node, const ConstraintEdge* store);
        
        bool processLoad(NodeID node, const ConstraintEdge* load);

        bool processCopy(NodeID node, const ConstraintEdge* edge);

        bool processGepPts(const PointsTo& pts, const GepCGEdge* edge);

        bool processGep(NodeID node, const GepCGEdge* edge);

        void handleCopyGep(ConstraintNode* node);

        void handleLoadStore(ConstraintNode *node);

        void pointToAnalysis();
    };

}
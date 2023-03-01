#ifndef FC_WITH_FLAT_EDGE_H_
#define FC_WITH_FLAT_EDGE_H_

#include <vector>
#include "basic_gene_fc.h"

class FcWithFlatEdge: public BasicGeneFc{
    public:
        FcWithFlatEdge(const int switchIn, const int layerIn, const int totalPortIn, const std::vector<int>& upDownIn, const std::vector<int>& flatIn);
        ~FcWithFlatEdge(){};
        void GeneTopo() override;
    private:
        int switches;
        int layerNum;
        int totalUpPort;
        std::vector<int> upDownDegree; 
        std::vector<int> flatEdgeNum; 
};


#endif  // FC_WITH_FLAT_EDGE_H_
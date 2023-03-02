#ifndef FC_WITH_FLAT_EDGE_H_
#define FC_WITH_FLAT_EDGE_H_

#include <vector>
#include <numeric>
#include <random>
#include <cstring>
#include <algorithm>
#include "utils.h"
#include "basic_gene_fc.h"

class FcWithFlatEdge: public BasicGeneFc{
    public:
        FcWithFlatEdge(const int switchIn, const int layerIn, const int totalPortIn, const std::vector<int>& upDownIn, const std::vector<int>& flatIn);
        ~FcWithFlatEdge(){};
        void GeneTopo() override;
        void StartFastMode(){fastTopoBuild = true;}
    private:
        void GeneUpDownTopo(std::vector<std::vector<int> > &possibleConnect);
    private:
        int switches;
        int layerNum;
        int totalUpPort;
        bool fastTopoBuild = false;
        std::vector<int> upDownDegree; 
        std::vector<int> flatEdgeLayerNum;

        std::vector<SwLink> linkInfor;

};


#endif  // FC_WITH_FLAT_EDGE_H_
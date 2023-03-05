#ifndef FC_UNDIRECTED_
#define FC_UNDIRECTED_
#include "basic_gene_fc.h"


class FcUndirected: public BasicGeneFc{
    public:
        FcUndirected(const int switchIn, const int layerIn, const int totalPortIn);
        ~FcUndirected();
        void GeneTopo() override;
        void ChangeRand(int randSeed){
            randomSeed = randSeed;
        }
    
    private:
        void GeneVirtualLink(std::vector<std::vector<int> >& allDegrees);
        void ConpressGraph(const std::vector<int>& heads, const std::vector<Edge>& edges, int edgeCount);
    
    private:
        int switches;
        int layerNum;
        int totalUpPort;
        int maxEdgeNum;
        int edgeBetSwNum;
        int randomSeed;
        std::vector<SwLink> linkInfor;


     

};


#endif
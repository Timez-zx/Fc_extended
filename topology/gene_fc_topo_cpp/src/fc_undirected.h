#ifndef FC_UNDIRECTED_
#define FC_UNDIRECTED_
#include "basic_gene_fc.h"


class FcUndirected: public BasicGeneFc{
    public:
        FcUndirected(const int switchIn, const int layerIn, const int totalPortIn);
        ~FcUndirected();
        void GeneTopo() override;
    
    private:
        void GeneVirtualLink();
    
    private:
        int switches;
        int layerNum;
        int totalUpPort;
        int maxEdgeNum;
        int edgeBetSwNum;
        std::vector<SwLink> linkInfor;


     

};


#endif
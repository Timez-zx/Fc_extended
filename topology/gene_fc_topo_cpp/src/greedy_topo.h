#ifndef GREEDY_TOPO_
#define GREEDY_TOPO_
#include "basic_gene_fc.h"



class GreedyTopo: public BasicGeneFc{
    public:
        GreedyTopo(const int switchIn, const int totalPortIn);
        ~GreedyTopo();
        void GeneTopo() override;
        void ChangeRand(int randSeed){
            randomSeed = randSeed;
        }
    private:
        int switches;
        int totalUpPort;
        int randomSeed;
    
};


#endif
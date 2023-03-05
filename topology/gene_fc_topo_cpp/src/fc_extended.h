#ifndef FC_EXTENDED_
#define FC_EXTENDED_
#include "basic_gene_fc.h"



class FcExtended: public BasicGeneFc{
    public:
        FcExtended(const int switchIn, const int layerIn, const int totalPortIn, const std::vector<int>& layerDegreeIn);
        ~FcExtended();
        void GeneTopo() override;
        void ChangeRand(int randSeed){
            randomSeed = randSeed;
        }
        void TopoBuildMode(int ifFast){
            fastTopoBuild = ifFast;
        }
        void GetCycleEdge();
    private:
        void GeneLink(std::vector<std::vector<int> > &possibleConnect);
        int GetVertexLabel(int swLabel, int layer, int ifDown);
    
    
    private:
        int switches;
        int layerNum;
        int totalUpPort;
        int randomSeed;
        int fastTopoBuild;
        std::vector<SwLink> linkInfor;
        std::vector<int> layerDegrees; 


     

};


#endif
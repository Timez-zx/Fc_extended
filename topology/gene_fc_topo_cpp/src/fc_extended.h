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
        void SaveTopoInfor();
    private:
        void GeneLink(std::vector<std::vector<int> > &possibleConnect);
        int GetVertexLabel(int swLabel, int layer, int ifDown);
        int GetSwLabel(int vertexLabel);
        void DFS(const std::vector<int>& heads, const std::vector<Edge>& edges, int start, int end);
        std::string GenePath(const std::string &path);
    
    
    private:
        int switches;
        int layerNum;
        int totalUpPort;
        int randomSeed;
        int fastTopoBuild;
        std::vector<SwLink> linkInfor;
        std::vector<int> layerDegrees; 

        std::vector<int> visitedGlobal;
        std::vector<int> stackGlobal;
        int maxNodePass;

        std::unordered_map<int,int> swTovirLayer;
        std::string topoPath;


     

};


#endif
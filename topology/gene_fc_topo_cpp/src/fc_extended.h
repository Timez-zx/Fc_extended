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
        void GetCycleEdge(int layer);
        void SaveTopoInfor();
        uint16_t SearchKsp(int src, int dst, int pathNum, int vcNum, uint16_t *pathInfor, int threadLabel);
        void MthreadKsp(int threadNum, int pathNum, int vcNum, bool ifReport, int reportInter);
        double throughputTest(const std::string& type, int seed, int pathNum, int vcNum, int hosts);
    private:
        void GeneLink(std::vector<std::vector<int> > &possibleConnect);
        int GetVertexLabel(int swLabel, int layer, int ifDown);
        int GetSwLabel(int vertexLabel);
        void DFS(const std::vector<int>& heads, const std::vector<Edge>& edges, int start, int end);
        std::string GenePath(const std::string &path);
        std::string GenePathKsp(const std::string& path, int pathNum, int vcNum);
        void ThreadKsp(const std::vector<Pair> &routePairs, int threadLabel, int pathNum, int vcNum, bool ifReport, int reportInter, std::string storeFile);
        void GeneWorseCase(float **flowMatrix, int hosts);
        void GeneUniformRandom(float **flowMatrix, int seed, int hosts);
    
    private:
        int switches;
        int layerNum;
        int totalUpPort;
        int randomSeed;
        int fastTopoBuild;
        int** bitMap=NULL;
        std::vector<SwLink> linkInfor;
        std::vector<int> layerDegrees; 

        std::vector<int> visitedGlobal;
        std::vector<int> stackGlobal;
        int maxNodePass;

        std::unordered_map<int,int> swTovirLayer;
        std::unordered_map<int,int> swPairToLayerPair;
        std::string topoPath;
        Graph **graphPr=NULL;
};


#endif
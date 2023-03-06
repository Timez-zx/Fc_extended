#ifndef FC_WITH_FLAT_EDGE_H_
#define FC_WITH_FLAT_EDGE_H_

#include "basic_gene_fc.h"



class FcWithFlatEdge: public BasicGeneFc{
    public:
        FcWithFlatEdge(const int switchIn, const int layerIn, const int totalPortIn, const std::vector<int>& upDownIn, const std::vector<int>& flatIn);
        ~FcWithFlatEdge();
        void GeneTopo() override;
        void StartFastMode(){fastTopoBuild = true;}
        void ChangeRandomSeed(int seed){randomSeed = seed;}
        void SaveTopoInfor();
        void MthreadKsp(int threadNum, int pathNum, int vcNum, bool ifReport, int reportInter);
        double throughputTest(const std::string& type, int seed, int pathNum, int vcNum, int hosts);
    private:
        void GeneUpDownTopo(std::vector<std::vector<int> > &possibleConnect);
        void GeneFlatTopo(std::vector<std::vector<int> > &possibleConnect);
        std::string GenePathKsp(const std::string& path, int pathNum, int vcNum);
        std::string GenePath(const std::string& path);
        void ThreadKsp(const std::vector<Pair> &routePairs, int threadLabel, int pathNum, int vcNum, bool ifReport, int reportInter, std::string storeFile);
        uint16_t SearchKsp(int src, int dst, int pathNum, int vcNum, uint16_t *pathInfor, int threadLabel);

        void GeneUniformRandom(float **flowMatrix, int seed, int hosts);
        void GeneWorseCase(float **flowMatrix, int hosts);
    private:
        int switches;
        int layerNum;
        int totalUpPort;
        int randomSeed = 0;
        int** bitMap=NULL;
        bool fastTopoBuild = false;
        std::vector<int> upDownDegree; 
        std::vector<int> flatEdgeLayerNum;
        std::vector<SwLink> linkInfor;
        std::vector<int> edgeLabel;

        std::string topoPath;

        std::unordered_map<int,int> swPairToLayerPair;
        std::unordered_map<int,int> swPairToEdgeLabel;

        Graph **graphPr=NULL;

};


#endif  // FC_WITH_FLAT_EDGE_H_
#ifndef FC_WITH_FLAT_EDGE_H_
#define FC_WITH_FLAT_EDGE_H_

#include <vector>
#include <numeric>
#include <random>
#include <stack>
#include <cstring>
#include <algorithm>
#include "utils.h"
#include "basic_gene_fc.h"

typedef struct _Edge
{
    int nextEdgeIdex;
    int toNode;
    int srcNode;
} Edge;


bool DetectCycleStack(const std::vector<int>& heads, const std::vector<Edge>& edges, int start);

void AddEdges(std::vector<int>& heads, std::vector<Edge>& edges, int src, int dst, int &edgeCount);

void DeleLastEdges(std::vector<int>& heads, std::vector<Edge>& edges, int &edgeCount);


class FcWithFlatEdge: public BasicGeneFc{
    public:
        FcWithFlatEdge(const int switchIn, const int layerIn, const int totalPortIn, const std::vector<int>& upDownIn, const std::vector<int>& flatIn);
        ~FcWithFlatEdge(){};
        void GeneTopo() override;
        void StartFastMode(){fastTopoBuild = true;}
        void ChangeRandomSeed(int seed){randomSeed = seed;}
        void SaveTopoInfor();
    private:
        void GeneUpDownTopo(std::vector<std::vector<int> > &possibleConnect);
        void GeneFlatTopo(std::vector<std::vector<int> > &possibleConnect);
        std::string GenePathKsp(const std::string& path, int pathNum, int vcNum);
        std::string GenePath(const std::string& path);
    private:
        int switches;
        int layerNum;
        int totalUpPort;
        int randomSeed = 0;
        bool fastTopoBuild = false;
        std::vector<int> upDownDegree; 
        std::vector<int> flatEdgeLayerNum;
        std::vector<SwLink> linkInfor;

        std::string topoPath;

};


#endif  // FC_WITH_FLAT_EDGE_H_
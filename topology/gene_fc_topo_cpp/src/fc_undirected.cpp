#include "fc_undirected.h"

FcUndirected::FcUndirected(const int switchIn, const int layerIn, const int totalPortIn){
    switches = switchIn;
    layerNum = layerIn;
    totalUpPort = totalPortIn;
    maxEdgeNum = layerNum*switches-1;
    edgeBetSwNum = switches*totalUpPort/2;
    if(edgeBetSwNum/layerIn > switches-1)
        show("Bad para, Please give more layers!");
}

FcUndirected::~FcUndirected(){}


void FcUndirected::GeneTopo(){
    std::vector<int> degrees(switches);
    std::vector<int> interEdgesNum(layerNum);
    std::vector<std::vector<int> > possibleConnect(switches);
    std::vector<int> tempVec(switches);
    std::vector<int> acycleHeads(switches);
    std::vector<Edge> acycleEdges(switches*2); 
    int average = edgeBetSwNum/layerNum, edgeNumLayer, edgeNumCount;
    int src, dst;
    int aboveAveNum = edgeBetSwNum - average*layerNum;
    for(int i = 0; i < layerNum; i++){
        if(i < aboveAveNum)
            interEdgesNum[i] = average+1;
        else    
            interEdgesNum[i] = average;
    }
    for(int i = 0; i < switches; i++){
        degrees[i] = totalUpPort;
        acycleHeads[i] = -1;
    }
    for(int i = 0; i < switches; i++)
        tempVec[i] = i;
    for(int i = 0; i < switches; i++){
        possibleConnect[i].assign(tempVec.begin(), tempVec.end());
        RemoveVecEle(possibleConnect[i], i);
    }
    for(int i = 0; i < layerNum; i++){
        edgeNumLayer = interEdgesNum[i];
        edgeNumCount = 0;
        acycleHeads.clear();
        acycleEdges.clear();
        acycleEdges.resize(2*switches);
        acycleHeads.resize(switches);
        memset(&acycleHeads[0], 0xff, switches*sizeof(int));
        while(edgeNumCount < edgeNumLayer){


            edgeNumCount++;
        }
    }
    
}



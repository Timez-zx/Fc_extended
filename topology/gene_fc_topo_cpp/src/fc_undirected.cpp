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
    int average = edgeBetSwNum/layerNum;
    show(edgeBetSwNum);
    int aboveAveNum = edgeBetSwNum - average*layerNum;
    for(int i = 0; i < layerNum; i++){
        if(i < aboveAveNum)
            interEdgesNum[i] = average+1;
        else    
            interEdgesNum[i] = average;
    }
    PrintVectorInt(interEdgesNum);
    for(int i = 0; i < switches; i++)
        degrees[i] = totalUpPort;
    
    for(int i = 0; i < layerNum; i++){

    }
    
}



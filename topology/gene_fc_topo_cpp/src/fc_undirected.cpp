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
    std::vector<int> swList(switches);
    std::vector<std::vector<int> > allDegrees(switches, std::vector<int>(layerNum,0));
    int average = edgeBetSwNum/layerNum, edgeNumLayer, edgeNumCount, edgeCount;
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
        swList[i] = i;
    }
    for(int i = 0; i < switches; i++)
        tempVec[i] = i;
    for(int i = 0; i < switches; i++){
        possibleConnect[i].assign(tempVec.begin(), tempVec.end());
        RemoveVecEle(possibleConnect[i], i);
    }
    srand(randomSeed);
    for(int i = 0; i < layerNum; i++){
        edgeNumLayer = interEdgesNum[i];
        edgeNumCount = 0;
        acycleHeads.clear();
        acycleEdges.clear();
        acycleEdges.resize(2*switches);
        acycleHeads.resize(switches);
        edgeCount = 0;
        memset(&acycleHeads[0], 0xff, switches*sizeof(int));
        while(edgeNumCount < edgeNumLayer){
            src = max_element(degrees.begin(), degrees.end()) - degrees.begin();
            dst = possibleConnect[src][rand()%possibleConnect[src].size()];
            while(!FindVecEle(swList, dst)){
                src = swList[rand()%swList.size()];
                dst = possibleConnect[src][rand()%possibleConnect[src].size()];
            }
            AddEdges(acycleHeads, acycleEdges, src, dst, edgeCount);
            if(DetectCycleStackUndirect(acycleHeads, acycleEdges, dst, src)){
                DeleLastEdges(acycleHeads, acycleEdges, edgeCount);
                continue;
            }
            else{
                AddEdges(acycleHeads, acycleEdges, dst, src, edgeCount);
                degrees[src]--;
                degrees[dst]--;
                allDegrees[src][i]++;
                allDegrees[dst][i]++;
                linkInfor.push_back(SwLink(SwNode(src, i), SwNode(dst, i)));
                linkInfor.push_back(SwLink(SwNode(dst, i), SwNode(src, i)));
                if(degrees[src] == 0)
                    RemoveVecEle(swList, src);
                if(degrees[dst] == 0)
                    RemoveVecEle(swList, dst);
                RemoveVecEle(possibleConnect[src], dst);
                RemoveVecEle(possibleConnect[dst], src);
                edgeNumCount++;
            }
        }
    }
    show("Real Edge finished!");
    GeneVirtualLink(allDegrees);
}


void FcUndirected::GeneVirtualLink(std::vector<std::vector<int> >& allDegrees){
    std::vector<int> acycleHeads(switches*layerNum, -1);
    std::vector<Edge> acycleEdges(switches*layerNum*10);
    std::vector<int> layerList(layerNum);
    std::vector<std::vector<int> > possibleConnect(layerNum);
    int srcHash, dstHash, edgeCount = 0, virEdgeCount, srcLayer, dstLayer;
    int averageEdge = floor((maxEdgeNum-edgeBetSwNum)/switches);
    for(auto link:linkInfor){
        srcHash = GetHash(link.srcNode.layerLabel, link.srcNode.swLabel, switches);
        dstHash = GetHash(link.dstNode.layerLabel, link.dstNode.swLabel, switches);
        AddEdges(acycleHeads, acycleEdges, srcHash, dstHash, edgeCount);
    }

    for(int i = 0; i < switches; i++){
        show(i);
        virEdgeCount = 0;
        possibleConnect.clear();
        possibleConnect.resize(layerNum);
        layerList.clear();
        layerList.resize(layerNum);
        InitVectorInt(layerList);
        for(int j = 0; j < layerNum; j++){
            possibleConnect[j].assign(layerList.begin(), layerList.end());
            RemoveVecEle(possibleConnect[j], j);
        }
        while(virEdgeCount < averageEdge){
            srcLayer = min_element(allDegrees[i].begin(), allDegrees[i].end()) - allDegrees[i].begin();
            dstLayer = possibleConnect[srcLayer][rand()%possibleConnect[srcLayer].size()];
            srcHash = GetHash(srcLayer, i, switches);
            dstHash = GetHash(dstLayer, i, switches);
            AddEdges(acycleHeads, acycleEdges, srcHash, dstHash, edgeCount);
            if(DetectCycleStackUndirect(acycleHeads, acycleEdges, dstHash, srcHash)){
                DeleLastEdges(acycleHeads, acycleEdges, edgeCount);
                RemoveVecEle(possibleConnect[srcLayer], dstLayer);
                RemoveVecEle(possibleConnect[dstLayer], srcLayer);
                if(possibleConnect[srcLayer].size() == 0)
                    RemoveVecEle(layerList, srcLayer);
                if(possibleConnect[dstLayer].size() == 0)
                    RemoveVecEle(layerList, dstLayer);
                continue;
            }
            else{
                AddEdges(acycleHeads, acycleEdges, dstHash, srcHash, edgeCount);
                RemoveVecEle(possibleConnect[srcLayer], dstLayer);
                RemoveVecEle(possibleConnect[dstLayer], srcLayer);
                if(possibleConnect[srcLayer].size() == 0)
                    RemoveVecEle(layerList, srcLayer);
                if(possibleConnect[dstLayer].size() == 0)
                    RemoveVecEle(layerList, dstLayer);
                allDegrees[i][srcLayer]++;
                allDegrees[i][dstLayer]++;
                virEdgeCount++;
            }
        }
        
    }



}

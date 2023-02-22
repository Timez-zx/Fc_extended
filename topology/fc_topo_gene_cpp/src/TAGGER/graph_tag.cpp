#include "graph_tag.h"

ReverseTaggerGraph::ReverseTaggerGraph(const std::string &filePath){
    FILE* ifs = fopen(filePath.c_str(), "r");
    fseek(ifs, 0, SEEK_END);
    int lenSize = ftell(ifs);
    rewind(ifs);
    int realSize = lenSize/4;
    int *allData = new int[realSize];
    int state = fread(allData, sizeof(int), realSize, ifs);
    int maxData = allData[realSize-3]*allData[realSize-2]*allData[realSize-1];
    heads = new TaggerNodeId[maxData];
    memset(heads, 0xff, maxData*sizeof(TaggerNodeId));
    edges = new SPLinkR[(realSize-3)/2];
    maxNode = maxData;
    mTag = allData[realSize-3];
    switchNum = allData[realSize-2];
    portNum = allData[realSize-1];
    edgeNum = (realSize-3)/2;
    int src,dst;
    for(int i = 0; i < (realSize-3)/2; i++){
        src = allData[2*i];
        dst = allData[2*i+1];
        edges[i].toNode = src;
        edges[i].nextEdgeIdex = heads[dst];
        heads[dst] = i;
    }
    fclose(ifs);
    delete allData;
    allData = NULL;
}


ReverseTaggerGraph::~ReverseTaggerGraph(){
    delete heads;
    heads = NULL;
    delete edges;
    edges = NULL;
}


ContractTaggerGraph::ContractTaggerGraph(const int &maxNode, const int &edgeNum){
    heads = new TaggerNodeId[maxNode];
    visited = new int[maxNode]();
    memset(heads, 0xff, maxNode*sizeof(TaggerNodeId));
    cmaxNode = maxNode;
    edges = new SPLink[edgeNum];
    edgesCount = 0;
}


ContractTaggerGraph::~ContractTaggerGraph(){
    delete heads;
    heads = NULL;
    delete edges;
    edges = NULL;
    delete visited;
    visited = NULL;
}


const bool ContractTaggerGraph::DetectCycle(const int& start, const int& end){
    for(int i = heads[start]; i != -1; i = edges[i].nextEdgeIdex){
        if(edges[i].toNode == end)
            return true;
        else{
            if(DetectCycle(edges[i].toNode, end))
                return true;
        }
    }
    return false;
}


const bool ContractTaggerGraph::DetectCycleStack(const int& start, const int& end){
    memset(visited, 0, cmaxNode*sizeof(int));
    std::stack<int> tempStack;
    int topVertex, nearVertex;
    tempStack.push(start);
    visited[start] = 1;
    while(!tempStack.empty()){
        topVertex = tempStack.top();
        tempStack.pop();
        for(int i = heads[topVertex]; i != -1; i = edges[i].nextEdgeIdex){
            nearVertex = edges[i].toNode;
            if(nearVertex == end)
                return true;
            else{
                if(!visited[nearVertex]){
                    tempStack.push(nearVertex);
                    visited[nearVertex] = 1;
                }
            }
        }
    }
    return false;
}


const int SearchMinTag::MinimumTag(){
    int maxNode = rGraph->GetMaxNode();
    int edgeNum = rGraph->GetEdgeNum();
    ContractTaggerGraph cGraph(maxNode, edgeNum);
    ContractTaggerGraph cGraphTemp(maxNode, edgeNum);
    int switchNum, portNum, maxTag, addEdgeCount, initTag = 0;
    int src, dst, state, changeTag = 0;
    SPLinkR tempEdge;
    SPLink tempEdgeC;
    switchNum = rGraph->GetSwitchNum();
    portNum = rGraph->GetPortNum();
    maxTag = maxNode/(switchNum*portNum);
    int *allNodeTag = new int[maxNode]();
    int cycleCount, sameEdgeCount, totalLayerEdge;
    for(int i = 0; i < maxTag ; i++){
        cycleCount = 0;
        sameEdgeCount = 0;
        totalLayerEdge = 0;
        for(int j = i*switchNum*portNum; j < (i+1)*switchNum*portNum; j++){
            dst = j%(switchNum*portNum)+allNodeTag[j]*switchNum*portNum;
            addEdgeCount = 0;
            for(int k = rGraph->GetHead(j); k != -1; k = tempEdge.nextEdgeIdex){
                totalLayerEdge++;
                tempEdge = rGraph->GetEdge(k);
                src = tempEdge.toNode%(switchNum*portNum)+allNodeTag[j]*switchNum*portNum;
                cGraphTemp.AddEdge(src, dst);
                addEdgeCount++;
            }
            state = cGraphTemp.DetectCycleStack(dst, dst);
            if(state){
                cycleCount++;
                for(int m = 0; m < addEdgeCount; m++)
                    cGraphTemp.DeleEdge();
                allNodeTag[j]++;
            }
            for(int k = rGraph->GetHead(j); k != -1; k = tempEdge.nextEdgeIdex){
                tempEdge = rGraph->GetEdge(k);
                src = tempEdge.toNode%(switchNum*portNum)+allNodeTag[j]*switchNum*portNum;
                cGraph.AddEdge(src, j%(switchNum*portNum)+allNodeTag[j]*switchNum*portNum);
            }
        }
        for(int j = i*switchNum*portNum; j < (i+1)*switchNum*portNum; j++){
            if(allNodeTag[j] != initTag){
                changeTag = 1;
                break;
            }
        }
        if(changeTag){
            std::cout << i << std::endl;
            for(int j = (initTag+1)*switchNum*portNum; j < (initTag+2)*switchNum*portNum; j++){
                src = j;
                for(int k = cGraph.GetHead(j); k != -1; k = tempEdgeC.nextEdgeIdex){
                    tempEdgeC = cGraph.GetEdge(k);
                    dst = tempEdgeC.toNode;
                    if(dst/(switchNum*portNum) == src/(switchNum*portNum)){
                        cGraphTemp.AddEdge(src, dst);
                        sameEdgeCount++;
                    }
                }

            }
            initTag++;
            for(int j = (i+1)*switchNum*portNum; j < maxNode; j++){
                allNodeTag[j] = initTag;
            }
            changeTag = 0;
        }
        std::cout << cycleCount << " " << sameEdgeCount << " " << totalLayerEdge << std::endl;
    }
    int minTag = 0;
    for(int i = 0; i < maxNode; i++){
        if(allNodeTag[i] > minTag)
            minTag = allNodeTag[i];
    }
    delete allNodeTag;
    allNodeTag = NULL;
    return minTag+1;
}


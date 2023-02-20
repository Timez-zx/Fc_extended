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
    edges = new SPLink[(realSize-3)/2];
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
    memset(heads, 0xff, maxNode*sizeof(TaggerNodeId));
    edges = new SPLink[edgeNum];
    edgesCount = 0;
}


ContractTaggerGraph::~ContractTaggerGraph(){
    delete heads;
    heads = NULL;
    delete edges;
    edges = NULL;
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


void SearchMinTag::MinTag(){
    minTag = 0;
    int maxNode = rGraph->GetMaxNode();
    int edgeNum = rGraph->GetEdgeNum();
    ContractTaggerGraph cGraph(maxNode, edgeNum);
    SPLink tempEdge;
    int dst, src;
    int switchNum, portNum;
    bool state;
    int addEdgeCount;
    int tempMinTag = minTag;
    switchNum = rGraph->GetSwitchNum();
    portNum = rGraph->GetPortNum();

    for(int i = 0; i < maxNode; i++){
        dst = i%(switchNum*portNum)+minTag*switchNum*portNum;
        addEdgeCount = 0;
        if(i%(switchNum*portNum) == 0 && minTag > tempMinTag)
            tempMinTag++;
        for(int j = rGraph->GetHead(i); j != -1; j = tempEdge.nextEdgeIdex){
            tempEdge = rGraph->GetEdge(j);
            src = tempEdge.toNode%(switchNum*portNum)+tempMinTag*switchNum*portNum;;
            cGraph.AddEdge(src, dst);
            addEdgeCount++;
            state = cGraph.DetectCycle(dst, src);
            if(state){
                for(int k = 0; k < addEdgeCount; k++)
                    cGraph.DeleEdge();
                minTag++;
                i--;
                break;
            }
        }
    }
}


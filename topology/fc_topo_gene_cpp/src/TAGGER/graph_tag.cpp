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


const int SearchMinTag::MinTag(){
    int maxNode = rGraph->GetMaxNode();
    int edgeNum = rGraph->GetEdgeNum();
    ContractTaggerGraph cGraph(maxNode, edgeNum);
    SPLink tempEdge;
    int dst, src, srcTag=0, dstTag=0;
    int switchNum, portNum;
    bool state;
    int addEdgeCount;
    int lastCycleLoca=0, tempToNode;
    switchNum = rGraph->GetSwitchNum();
    portNum = rGraph->GetPortNum();

    for(int i = 0; i < maxNode; i++){
        dst = i%(switchNum*portNum)+dstTag*switchNum*portNum;
        addEdgeCount = 0;
        // if(i%(switchNum*portNum) == 0 && dstTag > srcTag)
        //     srcTag++;
        for(int j = rGraph->GetHead(i); j != -1; j = tempEdge.nextEdgeIdex){
            tempEdge = rGraph->GetEdge(j);
            tempToNode = tempEdge.toNode;
            if(tempToNode >= lastCycleLoca)
                src = tempToNode%(switchNum*portNum)+dstTag*switchNum*portNum;
            else
                src = tempToNode%(switchNum*portNum)+(dstTag-1)*switchNum*portNum;
            cGraph.AddEdge(src, dst);
            addEdgeCount++;
            state = cGraph.DetectCycle(dst, src);
            if(state){
                for(int k = 0; k < addEdgeCount; k++)
                    cGraph.DeleEdge();
                dstTag++;
                lastCycleLoca = i;
                i--;
                break;
            }
        }
    }
    return dstTag + 1;
}


const int SearchMinTag::MinimumTag(){
    int maxNode = rGraph->GetMaxNode();
    int edgeNum = rGraph->GetEdgeNum();
    ContractTaggerGraph cGraph(maxNode, edgeNum);
    ContractTaggerGraph cGraphTemp(maxNode, edgeNum);
    int switchNum, portNum, maxTag, addEdgeCount, initTag = 0;
    int src, dst, state, changeTag = 0;
    SPLink tempEdge;
    switchNum = rGraph->GetSwitchNum();
    portNum = rGraph->GetPortNum();
    maxTag = maxNode/(switchNum*portNum);
    int *allNodeTag = new int[switchNum*portNum]();
    for(int i = 0; i < maxTag; i++){
        std::cout << initTag << std::endl;
        for(int j = i*switchNum*portNum; j < (i+1)*switchNum*portNum; j++){
            dst = j%(switchNum*portNum)+allNodeTag[j%(switchNum*portNum)]*switchNum*portNum;
            addEdgeCount = 0;
            for(int k = rGraph->GetHead(j); k != -1; k = tempEdge.nextEdgeIdex){
                tempEdge = rGraph->GetEdge(k);
                src = tempEdge.toNode%(switchNum*portNum)+allNodeTag[tempEdge.toNode%(switchNum*portNum)]*switchNum*portNum;
                cGraphTemp.AddEdge(src, dst);
                addEdgeCount++;
                state = cGraphTemp.DetectCycle(dst, src);
                if(state){
                    for(int m = 0; m < addEdgeCount; m++)
                        cGraphTemp.DeleEdge();
                    allNodeTag[j%(switchNum*portNum)]++;
                    break;
                }
            }
            for(int k = rGraph->GetHead(j); k != -1; k = tempEdge.nextEdgeIdex){
                tempEdge = rGraph->GetEdge(k);
                src = tempEdge.toNode%(switchNum*portNum)+allNodeTag[tempEdge.toNode%(switchNum*portNum)]*switchNum*portNum;
                cGraph.AddEdge(src, j%(switchNum*portNum)+allNodeTag[j%(switchNum*portNum)]*switchNum*portNum);
            }
        }
        for(int j = 0; j < switchNum*portNum; j++){
            if(allNodeTag[j] != initTag){
                changeTag = 1;
                break;
            }
        }
        if(changeTag){
            for(int j = (initTag+1)*switchNum*portNum; j < (initTag+2)*switchNum*portNum; j++){
                src = j;
                for(int k = cGraph.GetHead(j); k != -1; k = tempEdge.nextEdgeIdex){
                    tempEdge = cGraph.GetEdge(k);
                    dst = tempEdge.toNode;
                    if(dst/(switchNum*portNum) == src /(switchNum*portNum))
                        cGraphTemp.AddEdge(src, dst);
                }
            }
            initTag++;
            for(int j = 0; j < switchNum*portNum; j++){
                allNodeTag[j] = initTag;
            }
            changeTag = 0;
        }
    }
    int minTag = 0;
    for(int i = 0; i < switchNum*portNum; i++){
        if(allNodeTag[i] > minTag)
            minTag = allNodeTag[i];
    }
    delete allNodeTag;
    allNodeTag = NULL;
    return minTag+1;
}


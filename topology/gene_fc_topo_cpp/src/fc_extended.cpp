#include "fc_extended.h"





FcExtended::~FcExtended(){}


FcExtended::FcExtended(const int switchIn, const int layerIn, const int totalPortIn, const std::vector<int>& layerDegreeIn) {
    if(accumulate(layerDegreeIn.begin(), layerDegreeIn.end(),0) != totalPortIn){
        show("The number of ports is diff with the vec!");
        exit(1);
    }
    switches = switchIn;
    layerNum = layerIn;
    totalUpPort = totalPortIn;
    layerDegrees.assign(layerDegreeIn.begin(), layerDegreeIn.end());
}


void FcExtended::GeneTopo(){
    linkInfor.reserve(totalUpPort*switches);
    std::vector<std::vector<int> > possibleConnect(switches);
    std::vector<int> tempVec(switches);
    InitVectorInt(tempVec);
    for(int i = 0; i < switches; i++){
        possibleConnect[i].assign(tempVec.begin(), tempVec.end());
        RemoveVecEle(possibleConnect[i], i);
    }
    srand(randomSeed);
    GeneLink(possibleConnect);
}


void FcExtended::GeneLink(std::vector<std::vector<int> > &possibleConnect){
    int lastLayerRemainDegree = 0;
    int layerDegree, twoCount, zeroCount, deadCycleBreak, randIndex, minimum, tempCount;
    int src, dst;
    int *swDegrees = new int[switches];
    int *swDegreeLabel = new int[switches];
    bool **vertexConnect = new bool*[switches];
    std::vector<int> srcChoose;
    for(int i = 0; i < switches; i++)
        vertexConnect[i] = new bool[switches]();
    for(int i = layerNum - 1; i > 0; i--){
        layerDegree = layerDegrees[i] - lastLayerRemainDegree;
        for(int j = 0; j < layerDegree; j++){
            zeroCount = switches;
            twoCount = 0;
            memset(swDegrees, 0, sizeof(int)*switches);
            memset(swDegreeLabel, 0, sizeof(int)*switches);
            srcChoose.clear();
            srcChoose.resize(switches);
            for(int k = 0; k < switches; k++)
                srcChoose[k] = k;
            src = 0;
            while(twoCount < switches){
                deadCycleBreak = 0;
                randIndex = rand()%possibleConnect[src].size();
                dst = possibleConnect[src][randIndex];
                swDegrees[src]++;
                swDegrees[dst]++;
                if(swDegrees[src] == 1)
                    zeroCount--;
                if(swDegrees[dst] == 1)
                    zeroCount--;
                if(swDegrees[src] == 2)
                    twoCount++;
                if(swDegrees[dst] == 2)
                    twoCount++;
                while(vertexConnect[src][dst] || (twoCount+zeroCount==switches && zeroCount < 3 && srcChoose.size() != 1) || swDegreeLabel[dst]==1){
                    if(swDegrees[dst] == 2)
                        twoCount--;
                    if(swDegrees[dst] == 1)
                        zeroCount++;
                    swDegrees[dst]--;
                    randIndex = rand()%possibleConnect[src].size();
                    dst = possibleConnect[src][randIndex];
                    swDegrees[dst]++;
                    if(deadCycleBreak > 1e4){
                        std::cerr << "Can't construct, please change the rand seed or use other mode!" << std::endl;
                        exit(1);
                    }
                    deadCycleBreak++;
                    if(swDegrees[dst] == 1)
                        zeroCount--;
                    if(swDegrees[dst] == 2)
                        twoCount++;
                }
                vertexConnect[src][dst] = true;
                vertexConnect[dst][src] = true;
                swDegreeLabel[dst] = 1;
                linkInfor.push_back(SwLink(SwNode(src, i), SwNode(dst, i-1)));
                // linkInfor.push_back(SwLink(SwNode(dst, i-1), SwNode(src, i)));
                RemoveVecEle(possibleConnect[src], dst);
                RemoveVecEle(possibleConnect[dst], src);
                RemoveVecEle(srcChoose, src);
                if(fastTopoBuild)
                    src++;
                else{
                    minimum = switches+1;
                    for(int k = 0; k < srcChoose.size(); k++){
                        tempCount = 0;
                        for(int m = 0; m < switches; m++)
                            tempCount += swDegreeLabel[m] | vertexConnect[srcChoose[k]][m];
                        tempCount = switches - tempCount;
                        if(tempCount <= minimum){
                            src = srcChoose[k];
                            minimum = tempCount;
                        }
                    }
                }
            }
        }
        lastLayerRemainDegree = layerDegree;
    }
    delete [] swDegrees;
    delete [] swDegreeLabel;
    for(int i = 0; i <  switches; i++){
        delete[] vertexConnect[i];
    }
    delete[] vertexConnect;
    show(linkInfor.size());
    show("Topo constructed!");
}


void FcExtended::DFS(const std::vector<int>& heads, const std::vector<Edge>& edges, int start, int end){
    visitedGlobal[start] = 1;
    stackGlobal.push_back(start);
    int nearVertex;
    std::set<int> setVer;
    if(start == end){
        for(int i = 0; i < stackGlobal.size(); i++){
            setVer.insert(GetSwLabel(stackGlobal[i]));
        }
        PrintVectorInt(stackGlobal);
        if(setVer.size() > maxNodePass)
            maxNodePass = setVer.size();
        stackGlobal.pop_back();
        visitedGlobal[start] = 0;
        return;
    }
    for(int i = heads[start]; i != -1; i = edges[i].nextEdgeIdex){
        nearVertex = edges[i].toNode;
        if(!visitedGlobal[nearVertex]){
            DFS(heads, edges, nearVertex, end);
        }
    }
    stackGlobal.pop_back();
    visitedGlobal[start] = 0;

}


int FcExtended::GetVertexLabel(int swLabel, int layer, int ifDown){
    return ifDown*switches*layerNum+layer*switches+swLabel;
}

int FcExtended::GetSwLabel(int vertexLabel){
    return (vertexLabel%(switches*layerNum))%switches;
}


void FcExtended::GetCycleEdge(){
    int totalNode, maxEdgeNum, edgeCount = 0, initDegree = 0, lastIndex, realLayer;
    totalNode = switches*layerNum*2;
    maxEdgeNum = switches*layerNum*totalUpPort*10;
    std::vector<int> heads(totalNode, -1);
    std::vector<Edge> edges(maxEdgeNum);
    std::vector<int> linkIndex;
    linkIndex.push_back(0);
    lastIndex = 0;
    visitedGlobal.resize(totalNode);
    for(int i = layerNum-1; i > 0; i--){
        linkIndex.push_back((layerDegrees[i]-initDegree)*switches+lastIndex);
        lastIndex += (layerDegrees[i]-initDegree)*switches;
        initDegree = layerDegrees[i]-initDegree;
    }
    for(int i = 0; i < switches; i++)
        AddEdges(heads, edges, GetVertexLabel(i,layerNum-1,0), GetVertexLabel(i,layerNum-1,1), edgeCount);
    for(int i = 0; i < layerNum-1; i++){
        realLayer = layerNum-i-2;
        for(int j = 0; j < switches; j++){
            AddEdges(heads, edges, GetVertexLabel(j,realLayer,0), GetVertexLabel(j,realLayer+1,0), edgeCount);
            AddEdges(heads, edges, GetVertexLabel(j,realLayer+1,1), GetVertexLabel(j,realLayer,1), edgeCount);
        }
        for(int j = linkIndex[i]; j < linkIndex[i+1]; j++){
            AddEdges(heads, edges, GetVertexLabel(linkInfor[j].srcNode.swLabel,realLayer+1,1), GetVertexLabel(linkInfor[j].dstNode.swLabel,realLayer,1), edgeCount);
            AddEdges(heads, edges, GetVertexLabel(linkInfor[j].dstNode.swLabel,realLayer,0), GetVertexLabel(linkInfor[j].srcNode.swLabel,realLayer+1,0), edgeCount);
        }

        // memset(&visitedGlobal[0], 0, sizeof(int)*totalNode);
        // stackGlobal.clear();
        // maxNodePass = 0;
        // DFS(heads, edges, GetVertexLabel(2,realLayer,0), GetVertexLabel(2,realLayer,1));
        // show(maxNodePass);
    }

    
    
}
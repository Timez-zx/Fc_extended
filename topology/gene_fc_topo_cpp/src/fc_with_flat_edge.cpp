#include "fc_with_flat_edge.h"


void AddEdges(std::vector<int>& heads, std::vector<Edge>& edges, int src, int dst, int &edgeCount){
    edges[edgeCount].toNode = dst;
    edges[edgeCount].nextEdgeIdex = heads[src];
    edges[edgeCount].srcNode = src;
    heads[src] = edgeCount++;
}


void DeleLastEdges(std::vector<int>& heads, std::vector<Edge>& edges, int &edgeCount){
    edgeCount--;
    heads[edges[edgeCount].srcNode] = edges[edgeCount].nextEdgeIdex;
}


bool DetectCycleStack(const std::vector<int>& heads, const std::vector<Edge>& edges, int start){
    std::vector<int> visited(heads.size());
    memset(&visited[0], 0, heads.size()*sizeof(int));
    std::stack<int> tempStack;
    int topVertex, nearVertex;
    tempStack.push(start);
    visited[start] = 1;
    while(!tempStack.empty()){
        topVertex = tempStack.top();
        tempStack.pop();
        for(int i = heads[topVertex]; i != -1; i = edges[i].nextEdgeIdex){
            nearVertex = edges[i].toNode;
            if(nearVertex == start)
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


FcWithFlatEdge::FcWithFlatEdge(const int switchIn, const int layerIn, const int totalPortIn, const std::vector<int>& upDownIn, const std::vector<int>& flatIn) {
    switches = switchIn;
    layerNum = layerIn;
    totalUpPort = totalPortIn;
    upDownDegree.assign(upDownIn.begin(), upDownIn.end());
    flatEdgeLayerNum.assign(flatIn.begin(), flatIn.end());
}


void FcWithFlatEdge::GeneTopo() {
    int totalUpDownDegree = accumulate(upDownDegree.begin(), upDownDegree.end(),0);
    int maxFlatEdge = (totalUpPort-totalUpDownDegree)*switches/2;
    int flatEdgeNum = accumulate(flatEdgeLayerNum.begin(), flatEdgeLayerNum.end(),0);
    if(flatEdgeNum > maxFlatEdge){
        std::cerr << "Too many flat edges, Please change the number of flat edges, at most " << maxFlatEdge << "!\n";
        exit(1);
    }
    linkInfor.reserve(totalUpDownDegree*switches/2+flatEdgeNum);

    std::vector<std::vector<int> > possibleConnect(switches);
    std::vector<int> tempVec(switches);
    std::vector<int> srcChoose;
    int lastLayerRemainDegree = 0;
    int layerDegree, twoCount, zeroCount, deadCycleBreak, randIndex, minimum, tempCount;
    int src, dst;
    int *swDegrees = new int[switches];
    int *swDegreeLabel = new int[switches];
    bool **vertexConnect = new bool*[switches];
    for(int i = 0; i < switches; i++)
        tempVec[i] = i;
    for(int i = 0; i < switches; i++){
        possibleConnect[i].assign(tempVec.begin(), tempVec.end());
        RemoveVecEle(possibleConnect[i], i);
    }
    srand(randomSeed);
    GeneUpDownTopo(possibleConnect);
    GeneFlatTopo(possibleConnect);
}



void FcWithFlatEdge::GeneUpDownTopo(std::vector<std::vector<int> > &possibleConnect){
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
        layerDegree = upDownDegree[i] - lastLayerRemainDegree;
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
                    if(deadCycleBreak > 1e6){
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
                linkInfor.push_back(SwLink(SwNode(src, i+1), SwNode(dst, i)));
                linkInfor.push_back(SwLink(SwNode(dst, i), SwNode(src, i+1)));
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
    std::cout <<"Up down edge constructed!\n";
}


void FcWithFlatEdge::GeneFlatTopo(std::vector<std::vector<int> > &possibleConnect) {
    int totalUpDownDegree = accumulate(upDownDegree.begin(), upDownDegree.end(),0);
    int flatDegree = totalUpPort - totalUpDownDegree;
    int maxOutDegree = flatDegree/2,  maxInDegree = flatDegree/2;
    int src, dst, remainFlatEdge, layerCount = 0, deadCycleBreak=0, edgeCount;
    std::vector<int> outRemainDegrees(switches), inRemainDegrees(switches);
    std::vector<int> outDegreeSw(switches), inDegreeSw(switches), acycleHeads(switches);
    std::vector<Edge> acycleEdges(flatDegree*switches/2); 
    for(int i = 0; i < switches; i++){
        outRemainDegrees[i] = maxOutDegree;
        inRemainDegrees[i] = maxInDegree;
        outDegreeSw[i] = i;
        inDegreeSw[i] = i;
        acycleHeads[i] = -1;
    }
    while(layerCount < layerNum){
        remainFlatEdge = flatEdgeLayerNum[layerCount];
        edgeCount = 0;
        acycleHeads.clear();
        acycleEdges.clear();
        acycleEdges.resize(flatDegree*switches/2);
        acycleHeads.resize(switches);
        memset(&acycleHeads[0], 0xff, switches*sizeof(int));
        while(remainFlatEdge > 0){
            src = outDegreeSw[rand()%outDegreeSw.size()];
            dst = inDegreeSw[rand()%inDegreeSw.size()];
            while(!FindVecEle(possibleConnect[src], dst)){
                src = outDegreeSw[rand()%outDegreeSw.size()];
                dst = inDegreeSw[rand()%inDegreeSw.size()];
            }
            AddEdges(acycleHeads, acycleEdges, src, dst, edgeCount);
            if(DetectCycleStack(acycleHeads, acycleEdges, dst)){
                DeleLastEdges(acycleHeads, acycleEdges, edgeCount);
                continue;
            }
            else{
                linkInfor.push_back(SwLink(SwNode(src, (layerCount+1)), SwNode(dst, -1*(layerCount+1))));
                linkInfor.push_back(SwLink(SwNode(dst, -1*(layerCount+1)), SwNode(src, layerCount+1)));
                outRemainDegrees[src]--;
                inRemainDegrees[dst]--;
                if(outRemainDegrees[src] == 0)
                    RemoveVecEle(outDegreeSw, src);
                if(inRemainDegrees[dst] == 0)
                    RemoveVecEle(inDegreeSw, dst);
                RemoveVecEle(possibleConnect[src], dst);
                RemoveVecEle(possibleConnect[dst], src);
                remainFlatEdge--;
            }
        }
        layerCount++;
    }
}
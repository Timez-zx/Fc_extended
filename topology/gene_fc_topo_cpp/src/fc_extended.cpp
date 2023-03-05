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
                linkInfor.push_back(SwLink(SwNode(dst, i-1), SwNode(src, i)));
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


void FcExtended::GetCycleEdge(){
    int totalNode, maxEdgeNum, edgeCount = 0;
    totalNode = switches*layerNum*2;
    maxEdgeNum = switches*layerNum*totalUpPort*10;
    std::vector<int> heads(totalNode, -1);
    std::vector<Edge> edges(maxEdgeNum); 
    
}
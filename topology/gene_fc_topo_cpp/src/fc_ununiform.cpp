#include "fc_ununiform.h"

FcUnuniform::~FcUnuniform(){
    if(graphPr != NULL){
        delete[] graphPr;
        graphPr = NULL;
    }
    if (bitMap)
    {
        for(int i = 0; i < switches; i++){
            if(bitMap[i]){
                delete[] bitMap[i];
                bitMap[i] = NULL;
            }
        }   
        delete[] bitMap;
        bitMap = NULL;
    }    
}


FcUnuniform::FcUnuniform(const int switchIn, const int layerIn, const int totalPortIn, int maxLayerLabelIn, const std::vector<int>& layerDegreeIn) {
    if(accumulate(layerDegreeIn.begin(), layerDegreeIn.end(),0) != totalPortIn){
        show("The number of ports is diff with the vec!");
        exit(1);
    }
    switches = switchIn;
    layerNum = layerIn;
    totalUpPort = totalPortIn;
    maxLayerLabel = maxLayerLabelIn;
    layerDegrees.assign(layerDegreeIn.begin(), layerDegreeIn.end());
}


void FcUnuniform::GeneTopo(){
    int mediumSwNum, mediumLayer, averageLabel, layerChoose;
    linkInfor.reserve(totalUpPort*switches);
    std::vector<std::vector<int> > possibleConnect(switches);
    std::vector<std::vector<int> > layerSwitch(maxLayerLabel);
    std::vector<int> tempVec(switches);
    InitVectorInt(tempVec);
    srand(randomSeed);
    layerSwitch[0].resize(switches);
    InitVectorInt(layerSwitch[0]);
    layerSwitch[maxLayerLabel-1].assign(layerSwitch[0].begin(), layerSwitch[0].end());
    mediumSwNum = switches/(layerDegrees[1]/2);
    mediumLayer = layerNum-2;
    averageLabel = (maxLayerLabel-2)/mediumLayer;
    for(int i = 0; i < switches; i++){
        for(int j = 0; j < mediumLayer; j++){
            layerChoose = rand()%averageLabel+1+j*averageLabel;
            while(layerSwitch[layerChoose].size() >= mediumSwNum)
                layerChoose = rand()%averageLabel+1+j*averageLabel;
            layerSwitch[layerChoose].push_back(i);
        }
    }
    // for(int i = 0; i < maxLayerLabel; i++){
    //     PrintVectorInt(layerSwitch[i]);
    // }
    for(int i = 0; i < switches; i++){
        possibleConnect[i].assign(tempVec.begin(), tempVec.end());
        RemoveVecEle(possibleConnect[i], i);
    }

    bitMap = new int*[switches];
    for(int i = 0; i < switches; i++){
        bitMap[i] = new int[switches];
        memset(bitMap[i], 0, sizeof(int)*switches);
    }
    GeneLink(possibleConnect, layerSwitch);
}


void FcUnuniform::GeneLink(std::vector<std::vector<int> > &possibleConnect, std::vector<std::vector<int> > &layerSwitch){
    int upLayerDegree, downLayerDegree;
    int src, dst, lastdst, lastsrc;
    std::vector<int> degreeList(maxLayerLabel, layerDegrees[0]);
    std::vector<int> upLayerDegrees;
    std::vector<int> downLayerDegrees, downRemain;

    for(int i = 1; i < maxLayerLabel-1; i++)
        degreeList[i] = layerDegrees[1]/2;
    for(int i = 1; i < maxLayerLabel; i++){
        upLayerDegrees.clear();
        downLayerDegrees.clear();
        downRemain.clear();
        upLayerDegrees.assign(switches, degreeList[i]);
        downLayerDegrees.assign(switches, 0);
        for(int j = 0; j < layerSwitch[i-1].size(); j++)
            downLayerDegrees[layerSwitch[i-1][j]] = degreeList[i-1];
        downRemain.assign(layerSwitch[i-1].begin(), layerSwitch[i-1].end());
        for(int j = 0; j < layerSwitch[i].size(); j++){
            src = layerSwitch[i][j];
            for(int k = 0; k < upLayerDegrees[0]; k++){
                dst = max_element(downLayerDegrees.begin(), downLayerDegrees.end()) - downLayerDegrees.begin();
                // dst = downRemain[rand()%downRemain.size()];
                // dst = possibleConnect[src][rand()%possibleConnect[src].size()];
                while(!FindVecEle(possibleConnect[src], dst)){
                    if(downRemain.size() == 1){
                        // if(downLayerDegrees[lastdst] == 0)
                        downRemain.push_back(lastdst);
                        downLayerDegrees[lastdst]++;
                        possibleConnect[lastsrc].push_back(lastdst);
                        possibleConnect[lastdst].push_back(lastsrc);
                        // k--;
                    }
                    dst = downRemain[rand()%downRemain.size()];
                    // std::cout << src << " " << dst << " " << downRemain.size() << std::endl;
                }
                downLayerDegrees[dst]--;
                if(downLayerDegrees[dst] == 0)
                    RemoveVecEle(downRemain, dst);
                RemoveVecEle(possibleConnect[src], dst);
                RemoveVecEle(possibleConnect[dst], src);
                lastdst = dst;
                lastsrc = src;
            }
        }
    }
    // for(int i = 0; i < switches; i++){
    //     show(possibleConnect[i].size());
    // }

    show("Topo constructed!");
}
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

    for(int i = 0; i < maxLayerLabel; i++){
        PrintVectorInt(layerSwitch[i]);
    }

    for(int i = 0; i < switches; i++){
        possibleConnect[i].assign(tempVec.begin(), tempVec.end());
        RemoveVecEle(possibleConnect[i], i);
    }

    bitMap = new int*[switches];
    for(int i = 0; i < switches; i++){
        bitMap[i] = new int[switches];
        memset(bitMap[i], 0, sizeof(int)*switches);
    }
    GeneLink(possibleConnect);
}


void FcUnuniform::GeneLink(std::vector<std::vector<int> > &possibleConnect){

    show("Topo constructed!");
}
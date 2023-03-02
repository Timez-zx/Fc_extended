#include "fc_with_flat_edge.h"

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
    for(auto layerEdgeNum:flatEdgeLayerNum){
        if(layerEdgeNum >= switches){
            std::cerr << "For uniformity, the flat edge number of a layer should be less than switches number: " << switches << "!\n";
            exit(1);
        }
    }
    linkInfor.reserve(totalUpDownDegree*switches/2+flatEdgeNum);

    std::vector<std::vector<int> > possibleConnect(switches);
    std::vector<int> tempVec(switches);
    int lastLayerRemainDegree = 0;
    int layerDegree, twoCount, zeroCount, deadCycleBreak;
    int *swDegrees = new int[switches];
    for(int i = 0; i < switches; i++)
        tempVec[i] = i;
    for(int i = 0; i < switches; i++){
        possibleConnect[i].assign(tempVec.begin(), tempVec.end());
        RemoveVecEle(possibleConnect[i], i);
    }
    for(int i = layerNum - 1; i > 0; i--){
        layerDegree = upDownDegree[i] - lastLayerRemainDegree;
        for(int j = 0; j < layerDegree; j++){
            zeroCount = switches;
            twoCount = 0;
            memset(swDegrees, 0, sizeof(int)*switches);

            // while(twoCount < switches){
            //     deadCycleBreak = 0;


            // }
        }
        lastLayerRemainDegree = layerDegree;
    }

    delete [] swDegrees;

}
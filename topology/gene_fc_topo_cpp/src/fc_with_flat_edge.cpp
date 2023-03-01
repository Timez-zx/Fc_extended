#include "fc_with_flat_edge.h"

FcWithFlatEdge::FcWithFlatEdge(const int switchIn, const int layerIn, const int totalPortIn, const std::vector<int>& upDownIn, const std::vector<int>& flatIn) {
    switches = switchIn;
    layerNum = layerIn;
    totalUpPort = totalPortIn;
    upDownDegree.assign(upDownIn.begin(), upDownIn.end());
    flatEdgeLayerNum.assign(flatIn.begin(), flatIn.end());
}

void FcWithFlatEdge::GeneTopo() {
    int maxFlatEdge = (totalUpPort-accumulate(upDownDegree.begin(), upDownDegree.end(),0))*switches/2;
    int flatEdgeNum = accumulate(flatEdgeLayerNum.begin(), flatEdgeLayerNum.end(),0);
    if(flatEdgeNum > maxFlatEdge){
        std::cerr << "Too many flat edges, Please change the number of flat edge!\n";
        exit(1);
    }
    std::cout << maxFlatEdge << " " << flatEdgeNum <<"\n";

}
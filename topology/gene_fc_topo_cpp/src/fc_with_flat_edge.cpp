#include "fc_with_flat_edge.h"

FcWithFlatEdge::FcWithFlatEdge(const int switchIn, const int layerIn, const int totalPortIn, const std::vector<int>& upDownIn, const std::vector<int>& flatIn) {
    switches = switchIn;
    layerNum = layerIn;
    totalUpPort = totalPortIn;
    upDownDegree.assign(upDownIn.begin(), upDownIn.end());
    flatEdgeNum.assign(flatIn.begin(), flatIn.end());
}

void FcWithFlatEdge::GeneTopo() {

}
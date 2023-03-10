#include "src/greedy_topo.h"

GreedyTopo::~GreedyTopo(){
}


GreedyTopo::GreedyTopo(const int switchIn, const int totalPortIn){
    switches = switchIn;
    totalUpPort = totalPortIn;
}

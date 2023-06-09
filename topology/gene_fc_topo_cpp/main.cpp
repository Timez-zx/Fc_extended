#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include "src/fc_extended.h"
#include "src/fc_ununiform.h"
#include "src/greedy_topo.h"
using namespace std;




int main(int argc, char* argv[]){
    // int switches = 100;
    // int layerIn = 6;
    // int totalUpPort = 10;
    // int randomSeed = 1;
    // int mode = 0;
    // int layerAdd = 8;
    // if(argc == 4){
    //     switches = atoi(argv[1]);
    //     randomSeed = atoi(argv[2]);
    //     layerAdd = atoi(argv[3]);
    // }
    // std::vector<int> layerDegrees = {1, 2, 2, 2, 2, 1};
    // // std::vector<int> layerDegrees = {1, 2, 2, 2, 2, 2, 2, 2, 2, 1};

    // FcExtended fcTest(switches, layerIn, totalUpPort, layerDegrees);
    // fcTest.ChangeRand(randomSeed);
    // fcTest.TopoBuildMode(mode);
    // fcTest.GeneTopo();
    // fcTest.GetCycleEdge(layerAdd);
    // fcTest.SaveTopoInfor();
    // fcTest.MthreadKsp(16, 32, 1, 1, 100);
    // double throughput = fcTest.throughputTest("wr", 1, 32, 1, 14);

    int switches = 100;
    int layerIn = 4;
    int totalUpPort = 18;
    int randomSeed = 1;
    int mode = 0;
    int maxLayerLabel = 10;
    int seed = 1;
    if(argc == 4){
        switches = atoi(argv[1]);
        randomSeed = atoi(argv[2]);
        seed = atoi(argv[3]);
    }
    std::vector<int> layerDegrees = {1, 8, 8, 1};
    FcUnuniform fcTest(switches, layerIn, totalUpPort, maxLayerLabel,layerDegrees);
    for(int i = randomSeed; i < 1000; i++){
        fcTest.ChangeRand(i);
        fcTest.GeneTopo();
        if(fcTest.topoStatus){
            show(i);
            break;
        }
    }
    fcTest.GetCycleEdge(seed);
    struct timeval start, end;
    gettimeofday(&start, NULL);
    fcTest.SaveTopoInfor();
    fcTest.MthreadKsp(16, 32, 1, 1, 1000);
    gettimeofday(&end, NULL);
    double throughput = fcTest.throughputTest("wr", 1, 32, 1, 14);


    
    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}

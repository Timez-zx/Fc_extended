#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include "src/fc_extended.h"
#include "src/fc_ununiform.h"
using namespace std;




int main(int argc, char* argv[]){
    struct timeval start, end;
    gettimeofday(&start, NULL);
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
    if(argc == 3){
        switches = atoi(argv[1]);
        randomSeed = atoi(argv[2]);
    }
    std::vector<int> layerDegrees = {1, 8, 8, 1};
    FcUnuniform fcTest(switches, layerIn, totalUpPort, maxLayerLabel,layerDegrees);
    fcTest.ChangeRand(randomSeed);
    fcTest.TopoBuildMode(mode);
    fcTest.GeneTopo();
    fcTest.SaveTopoInfor();
    // fcTest.MthreadKsp(1, 32, 1, 1, 1);
    // double throughput = fcTest.throughputTest("wr", 2, 32, 2, 14);

    
    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}

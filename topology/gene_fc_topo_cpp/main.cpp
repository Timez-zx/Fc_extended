#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include "src/fc_extended.h"
using namespace std;




int main(int argc, char* argv[]){
    struct timeval start, end;
    gettimeofday(&start, NULL);
    int switches = 100;
    int layerIn = 10;
    int totalUpPort = 18;
    int randomSeed = 1;
    int mode = 0;
    int layerAdd = 8;
    if(argc == 4){
        switches = atoi(argv[1]);
        randomSeed = atoi(argv[2]);
        layerAdd = atoi(argv[3]);
    }
    // std::vector<int> layerDegrees = {1, 2, 2, 2, 2, 2, 4, 3};
    std::vector<int> layerDegrees = {1, 2, 2, 2, 2, 2, 2, 2, 2, 1};
    FcExtended fcTest(switches, layerIn, totalUpPort, layerDegrees);
    fcTest.ChangeRand(randomSeed);
    fcTest.TopoBuildMode(mode);
    fcTest.GeneTopo();
    fcTest.GetCycleEdge(layerAdd);
    fcTest.SaveTopoInfor();
    fcTest.MthreadKsp(16, 32, 1, 1, 1000);
    double throughput = fcTest.throughputTest("wr", 1, 32, 1, 14);

    
    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}

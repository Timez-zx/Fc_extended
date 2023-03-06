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
    if(argc == 4){
        switches = atoi(argv[1]);
        randomSeed = atoi(argv[2]);
        mode = atoi(argv[3]);
    }
    // std::vector<int> layerDegrees = {3, 6, 6, 3};
    std::vector<int> layerDegrees = {1, 2, 2, 2, 2, 2, 2, 2, 2, 1};
    FcExtended fcTest(switches, layerIn, totalUpPort, layerDegrees);
    fcTest.ChangeRand(randomSeed);
    fcTest.TopoBuildMode(mode);
    fcTest.GeneTopo();
    fcTest.GetCycleEdge();

    
    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}

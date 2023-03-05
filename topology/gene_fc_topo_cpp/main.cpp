#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include "src/fc_undirected.h"
#include "src/fc_with_flat_edge.h"
using namespace std;




int main(int argc, char* argv[]){
    struct timeval start, end;
    gettimeofday(&start, NULL);
    // int switches = 200;
    int randSeed = 1;
    if(argc == 2){
        randSeed = atoi(argv[1]);
    }
    // int layerNum = 5;
    // int totalPort = 18;
    // vector<int> upDownDegree = {1, 2, 2, 2, 1};
    // vector<int> flatEdge = {55, 55, 55, 55, 30};
    // for(int i = 0; i < layerNum; i++)
    //     flatEdge[i] *= switches/50;
    // FcWithFlatEdge fcFlat(switches, layerNum, totalPort, upDownDegree, flatEdge);
    // // fcFlat.StartFastMode();
    // fcFlat.ChangeRandomSeed(randSeed);
    // fcFlat.GeneTopo();
    // fcFlat.SaveTopoInfor();
    // fcFlat.MthreadKsp(16, 32, 1, 1, 1000);
    // double thPut = 0;
    // thPut += fcFlat.throughputTest("wr", 1, 32, 1, 14);
    // show(thPut);
    int switches = 1000;
    int layerIn = 20;
    int portUpNum = 18;
    FcUndirected fcTest(switches, layerIn, portUpNum);
    fcTest.ChangeRand(randSeed);
    fcTest.GeneTopo();

    
    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}

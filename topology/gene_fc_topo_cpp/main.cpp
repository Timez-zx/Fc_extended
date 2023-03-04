#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include "src/fc_undirected.h"
#include "src/fc_with_flat_edge.h"
using namespace std;




int main(int argc, char* argv[]){
    struct timeval start, end;
    gettimeofday(&start, NULL);
    // int switches = 400;
    // int randSeed = 1;
    // if(argc == 3){
    //     switches = atoi(argv[1]);
    //     randSeed = atoi(argv[2]);
    // }
    // int layerNum = 10;
    // int totalPort = 18;
    // vector<int> upDownDegree = {1, 2, 2, 2, 2, 2, 2, 2, 2, 1};
    // vector<int> flatEdge = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    // for(int i = 0; i < layerNum; i++)
    //     flatEdge[i] *= switches/50;
    // FcWithFlatEdge fcFlat(switches, layerNum, totalPort, upDownDegree, flatEdge);
    // // fcFlat.StartFastMode();
    // fcFlat.ChangeRandomSeed(randSeed);
    // fcFlat.GeneTopo();
    // fcFlat.SaveTopoInfor();
    // fcFlat.MthreadKsp(16, 32, 100, 1, 1000);
    // double thPut = 0;
    // for(int i = 1; i < 11; i++){
    //     thPut += fcFlat.throughputTest("ur", i, 32, 100, 14);
    // }
    // show(thPut);
    int switches = 50;
    int layerIn = 20;
    int portUpNum = 18;
    FcUndirected fcTest(switches, layerIn, portUpNum);
    fcTest.GeneTopo();

    
    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}

#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include "src/fc_with_flat_edge.h"
using namespace std;




int main(int argc, char* argv[]){
    int switches = 400;
    int randSeed = 1;
    if(argc == 3){
        switches = atoi(argv[1]);
        randSeed = atoi(argv[2]);
    }
    struct timeval start, end;
    gettimeofday(&start, NULL);
    int layerNum = 10;
    int totalPort = 18;
    vector<int> upDownDegree = {1, 2, 2, 2, 2, 2, 2, 2, 2, 1};
    vector<int> flatEdge = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for(int i = 0; i < layerNum; i++)
        flatEdge[i] *= switches/50;
    FcWithFlatEdge fcFlat(switches, layerNum, totalPort, upDownDegree, flatEdge);
    // fcFlat.StartFastMode();
    fcFlat.ChangeRandomSeed(randSeed);
    fcFlat.GeneTopo();
    fcFlat.SaveTopoInfor();
    fcFlat.MthreadKsp(16, 32, 1, 1, 1000);
    fcFlat.throughputTest("wr", 1, 32, 1, 14);
    
    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}

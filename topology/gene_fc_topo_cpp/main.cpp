#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include "src/fc_with_flat_edge.h"
using namespace std;




int main(){
    struct timeval start, end;
    gettimeofday(&start, NULL);
    int switches = 200;
    int layerNum = 7;
    int totalPort = 18;
    vector<int> upDownDegree = {1, 2, 2, 2, 2, 2, 1};
    vector<int> flatEdge = {100, 100, 100, 100, 100, 100, 0};
    FcWithFlatEdge fcFlat(switches, layerNum, totalPort, upDownDegree, flatEdge);
    // fcFlat.StartFastMode();
    fcFlat.ChangeRandomSeed(5);
    fcFlat.GeneTopo();
    fcFlat.SaveTopoInfor();
    fcFlat.MthreadKsp(2, 32, 2, 1, 1000);
    
    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}

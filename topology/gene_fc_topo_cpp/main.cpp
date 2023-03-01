#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include "src/fc_with_flat_edge.h"
using namespace std;




int main(){
    struct timeval start, end;
    gettimeofday(&start, NULL);
    int switches = 100;
    int layerNum = 4;
    int totalPort = 18;
    vector<int> upDownDegree = {2, 4, 4, 2};
    vector<int> flatEdge = {10, 10, 10, 10};
    FcWithFlatEdge fcFlat(switches, layerNum, totalPort, upDownDegree, flatEdge);


    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}
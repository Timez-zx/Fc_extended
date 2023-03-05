#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include "src/fc_undirected.h"
#include "src/fc_with_flat_edge.h"
using namespace std;




int main(int argc, char* argv[]){
    struct timeval start, end;
    gettimeofday(&start, NULL);


    
    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}

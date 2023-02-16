#include <sys/time.h>
#include "src/fc_edge_disjoin_route.h"
#include "src/fc_edge_disjoin_cost_route.h"
#include "src/fc_tagger_ksp.h"


// There are some demos in the demo, please refer file "demo". 

int main(){
    struct timeval start, end;
    gettimeofday(&start, NULL);

    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}

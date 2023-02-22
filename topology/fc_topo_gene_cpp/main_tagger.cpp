#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include "src/TAGGER/graph_tag.h"
 

using namespace std;




int main(){
    struct timeval start, end;
    gettimeofday(&start, NULL);

    SearchMinTag minT("/home/zx/Fc_exp/fc_extended/topology/fc_topo_gene_cpp/data/tagger_infor/sw100_vir2662_rand5/sw100_vir2662_rand5_ksp32_vc100");
    cout << minT.MinimumTag() << " " << minT.GetMaxTag() << endl;

    

    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}

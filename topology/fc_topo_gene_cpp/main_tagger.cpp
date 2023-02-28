#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include "src/TAGGER/graph_tag.h"
 

using namespace std;




int main(){
    struct timeval start, end;
    gettimeofday(&start, NULL);

    SearchMinTag minT("/home/zx/Fc_exp/fc_extended/topology/fc_topo_gene_cpp/data/tagger_infor/sw5000_vir3663_rand3/sw5000_vir3663_rand3_ksp32_vc100");
    cout << minT.MiniTag() << " " << minT.GetMaxTag() << endl;


    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}

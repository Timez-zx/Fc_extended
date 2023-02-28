#include <sys/time.h>
#include <iostream>
#include <unistd.h>
// #include "src/fc_base.h"
#include "src/TAGGER/graph_tag.h"
 

using namespace std;




int main(){
    struct timeval start, end;
    gettimeofday(&start, NULL);

    SearchMinTag minT("/home/zx/Fc_exp/fc_extended/topology/fc_topo_gene_cpp/data/tagger_infor/sw5000_vir3663_rand3/sw5000_vir3663_rand3_ksp32_vc100");
    cout << minT.MiniTag() << " " << minT.GetMaxTag() << endl;

    // int serverL[] = {1152, 2400, 4800, 7200, 24000, 48000, 72000};
    // int hostL[] = {8, 12, 16};
    // int virL[][4] = {{4,8,8,4}, {3,7,7,3}, {2,6,6,2}};
    // int colL[] = {9, 7, 6, 14, 11, 9, 19, 15, 13, 24, 19, 16, 53, 38, 29, 90, 59, 47, 129, 80, 61};
    // for(int i = 0; i < 7; i++){
    //     for(int j = 0; j < 3; j++){
    //         int switches = serverL[i]/hostL[j];
    //         int hosts = hostL[j];
    //         int ports = 32;
    //         int *vir_layer_degree = virL[j];
    //         int layer_num = 4;
    //         int is_random = 1;
    //         int random_seed = 4;
    //         // All class is ok
    //         Fc_base fc_test(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed);
    //         // If the number of switches are larger, use para 1
    //         if(switches <= 1000)
    //             fc_test.fc_topo_gene_1v1(0);
    //         else
    //             fc_test.fc_topo_gene_1v1(1);

    //         int ocs_ports = 320;
    //         int distance_infor[4];
    //         distance_infor[0] = 2; // Tor to tor:x
    //         distance_infor[1] = 2; // Tor to tor:y
    //         distance_infor[2] = 2; // Ocs to tor:x
    //         distance_infor[3] = 2;  // Ocs to ocs:y
    //         int column_num = colL[i*3+j];
    //         fc_test.cost_model(ocs_ports, distance_infor, column_num);
    //     }
    // }

    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}

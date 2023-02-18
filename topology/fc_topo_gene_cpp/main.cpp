#include <sys/time.h>
#include "src/fc_edge_disjoin_route.h"
#include "src/fc_edge_disjoin_cost_route.h"
#include "src/fc_tagger_kspz.h"


// There are some demos in the demo, please refer file "demo".
double up_down_ksp_throught(Fc_topo_all_route& fc){
    fc.fc_topo_gene_1v1(0);
    fc.path_infor_gene();
    fc.build_search_dic();
    bool if_report = true;
    int report_inter = 5000;
    bool if_store = true;
    string mode = "ksp";
    int ksp_num = 32;
    fc.pthread_for_all_path(16, if_report, report_inter, if_store, mode, ksp_num);
    return fc.throughput_test("wr", 2);
} 


double edge_disjoint_throught(Fc_edge_disjoin_route& fc){
    fc.fc_topo_gene_1v1(0);
    fc.path_infor_gene();
    fc.build_search_dic();
    bool if_report = true;
    int report_inter = 5000;
    bool if_store = true;
    bool store_part = false;
    fc.pthread_for_all_route(16, if_report, report_inter, if_store, store_part);
    fc.find_all_route(16, 5000);
    return fc.throughput_test("wr", 2);
} 


double ksp_tagger_throught(Fc_tagger_kspz& fc, int path_num, int vc_num){
    fc.fc_topo_gene_1v1(0);
    fc.save_graph_infor();
    bool if_report = true;
    int report_inter = 100000;
    bool if_store = true;
    bool store_part = false;
    fc.pthread_up_down_ksp(16, path_num, vc_num, if_report, report_inter, if_store);
    return fc.throughput_test_ksp("wr", 2, path_num, vc_num);
}


int main(){
    struct timeval start, end;
    gettimeofday(&start, NULL);
    int vir_layer_degrees[3][4] = {{4,8,8,4}, {2,8,8,2}, {2,6,6,2}};
    int host[3] = {8, 12, 16};
    int server[3] = { 24000, 48000, 72000};
    for(int m = 0; m < 3; m++){
        for(int n = 0; n < 3; n++){
            int switches = server[m]/host[n];
            int hosts = host[n];
            int ports = 32;
            int layer_num = 4;
            int is_random = 1;
            int random_seed = 5;
            // All class is ok
            Fc_tagger_kspz fc_test(switches, hosts, ports, vir_layer_degrees[n], layer_num, is_random, random_seed);
            // If the number of switches are larger, use para 1
            fc_test.fc_topo_gene_1v1(1);
            int min_fiber = 1e9;
            int min_ocs;
            int min_colomn;

            int ocs_ports = 400;
            int distance_infor[4];
            distance_infor[0] = 2; // Tor to tor:x
            distance_infor[1] = 2; // Tor to tor:y
            distance_infor[2] = 2; // Ocs to tor:x
            for(int i = 2; i < 3 ; i++){
                distance_infor[3] = i;  // Ocs to ocs:y
                for(int j = 10; j <= 150; j++){
                    int min_f = fc_test.cost_model(ocs_ports, distance_infor, j);
                    if(min_f < min_fiber){
                        min_fiber = min_f;
                        min_ocs = i;
                        min_colomn = j;
                    }
                }
            }
            cout << switches << " " <<server[m] << " "<<min_ocs << " " << min_colomn << " " << min_fiber << endl;
        }
    }
    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}

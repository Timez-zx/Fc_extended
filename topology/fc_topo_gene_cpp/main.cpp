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
    int switches = 100;
    int hosts = 14;
    int ports = 32;
    int vir_layer_degree[] = {1, 2, 2, 2, 2, 2, 2, 2, 2, 1};
    int layer_num = 10;
    int is_random = 1;
    int random_seed = 5;
    Fc_tagger_kspz fc_test(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed);
    fc_test.fc_topo_gene_1v1(0);
    fc_test.save_graph_infor();
    bool if_report = true;
    int report_inter = 10000;
    bool if_store = true;
    bool store_part = false;
    // fc_test.pthread_up_down_ksp(16, 32, 2, if_report, report_inter, if_store);
    // fc_test.pthread_up_down_ksp(16, 32, 3, if_report, report_inter, if_store);
    fc_test.pthread_up_down_ksp(1, 32, 2, if_report, report_inter, if_store);
    // double wr_ave = fc_test.throughput_test_ksp("wr", 2, 32, 100);
    // wr_ave = fc_test.throughput_test_ksp("wr", 2, 32, 3);
    // wr_ave = fc_test.throughput_test_ksp("wr", 2, 32, 2);


    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}

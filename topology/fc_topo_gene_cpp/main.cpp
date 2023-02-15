#include <sys/time.h>
#include "src/fc_edge_disjoin_route.h"
#include "src/fc_edge_disjoin_cost_route.h"
#include "src/fc_tagger_ksp.h"

int main(){
    struct timeval start, end;
    gettimeofday(&start, NULL);
    // int switches = 2000;
    // int hosts = 24;
    // int ports = 64;
    // int vir_layer_degree[] = {5, 10, 10, 10, 5};
    // int layer_num = 5;
    // int is_random = 1;
    // int random_seed = 2;

    // int switches = 500;
    // int hosts = 24;
    // int ports = 64;
    // int vir_layer_degree[] = {7, 13, 13, 7};
    // int layer_num = 4;
    // int is_random = 1;
    // int random_seed = 1;

    int switches = 144;
    int hosts = 8;
    int ports = 32;
    int vir_layer_degree[] = {4, 8, 8, 4};
    int layer_num = 4;
    int is_random = 1;
    int random_seed = 1;


    // int switches = 6000;
    // int hosts = 12;
    // int ports = 32;
    // int vir_layer_degree[] = {3, 7, 7, 3};
    // int layer_num = 4;
    // int is_random = 1;
    // int random_seed = 2;

    // int switches = 4500;
    // int hosts = 16;
    // int ports = 32;
    // int vir_layer_degree[] = {2, 6, 6, 2};
    // int layer_num = 4;
    // int is_random = 1;
    // int random_seed = 1;

    // int switches = 100;
    // int hosts = 14;
    // int ports = 32;
    // int vir_layer_degree[] = {2, 3, 4, 4, 3, 2};
    // int layer_num = 6;
    // int is_random = 1;
    // int random_seed = 5;
    // Fc_edge_disjoin_cost_route fc_test(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed);
    Fc_tagger_ksp fc_test(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed);
    fc_test.fc_topo_gene_1v1(0);
    // fc_test.save_graph_infor();
    // fc_test.path_infor_gene();
    // fc_test.build_search_dic();


    bool if_report = true;
    int report_inter = 100;
    bool if_store = true;
    bool store_part = false;
    // fc_test.pthread_up_down_ksp(16, 32, 2, if_report, report_inter, if_store);
    // fc_test.pthread_up_down_ksp(16, 32, 3, if_report, report_inter, if_store);
    // fc_test.pthread_up_down_ksp(16, 32, 100, if_report, report_inter, if_store);
    // fc_test.pthread_for_all_route(4, if_report, report_inter, if_store, store_part);
    // string mode = "ksp";
    // int ksp_num = 32;
    // fc_test.pthread_for_all_path(1, if_report, report_inter, if_store, mode, ksp_num);

    // bool if_search_map = true;
    // fc_test.find_all_route(4, 5000, if_search_map);
    // double wr_ave = fc_test.throughput_test("wr", 2);
    // double wr_ave = fc_test.throughput_test("wr", 2, 32, 2);

    int ocs_ports = 400;
    int distance_infor[4];
    distance_infor[0] = 2; // Tor to tor:x
    distance_infor[1] = 2; // Tor to tor:y
    distance_infor[2] = 2; // Ocs to tor:x
    distance_infor[3] = 2;  // Ocs to ocs:y
    int column_num = 50;
    fc_test.cost_model(ocs_ports, distance_infor, column_num);

    // fc_test.multi_thread_b_bandwidth(16, 50, 20, 1000);
    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}
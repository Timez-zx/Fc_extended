#include <sys/time.h>
#include "fc_edge_disjoin_route.h"
// #include "fc_edge_disjoin_cost_route.h"

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
    int random_seed = 2;

    // Fc_edge_disjoin_cost_route fc_test(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed);
    Fc_edge_disjoin_route fc_test(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed);
    fc_test.fc_topo_gene_1v1();
    // fc_test.fc_topo_gene();

    // fc_test.path_infor_gene();
    // fc_test.display_all_path();
    // fc_test.build_search_dic();
    // fc_test.display_dic(2);


    // bool if_report = true;
    // int report_inter = 5000;
    // bool if_store = true;
    // bool store_part = false;
    // fc_test.pthread_for_all_route(4, if_report, report_inter, if_store, store_part);
    // // fc_test.pthread_for_all_path(8, if_report, report_inter, if_store);

    // bool if_search_map = true;
    // fc_test.find_all_route(4, 5000, if_search_map);
    // double wr_ave = fc_test.throughput_test("wr", 1);


    // int ocs_ports = 400;
    // int distance_infor[4];
    // distance_infor[0] = 2; // Tor to tor:x
    // distance_infor[1] = 2; // Tor to tor:y
    // distance_infor[2] = 2; // Ocs to tor:x
    // distance_infor[3] = 2;  // Ocs to ocs:y
    // int copper_cost = 150;
    // int fiber_cost = 2;   // 2$/m
    // int tranceiver_cost[3];
    // tranceiver_cost[0] = 500;
    // tranceiver_cost[1] = 800;
    // tranceiver_cost[2] = 1000; 
    // fc_test.cost_model(ocs_ports, distance_infor, copper_cost, fiber_cost, tranceiver_cost);

    // int band;
    // int min_band = 1e7;
    // for(int i = 0; i < 1; i++){
    //     band = fc_test.bisection_bandwidth_byRand(i, 10000000);
    //     if(band < min_band)
    //         min_band = band;
    // }
    // cout << min_band << endl;

    fc_test.bisection_bandwidth_byExchange(1);
    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}
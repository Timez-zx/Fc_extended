#include <sys/time.h>
#include "fc_edge_disjoin_route.h"

int main(){
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
    // int random_seed = 7;

    int switches = 10;
    int hosts = 24;
    int ports = 36;
    int vir_layer_degree[] = {2, 4, 4, 2};
    int layer_num = 4;
    int is_random = 1;
    int random_seed = 2;

    Fc_edge_disjoin_route fc_test(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed);
    fc_test.fc_topo_gene();
    fc_test.path_infor_gene();
    // fc_test.display_all_path();
    fc_test.build_search_dic();
    // fc_test.display_dic(2);


    struct timeval start, end;
    gettimeofday(&start, NULL);
    bool if_report = true;
    int report_inter = 45;
    bool if_store = true;
    bool store_part = false;
    fc_test.pthread_for_all_route(2, if_report, report_inter, if_store, store_part);

    // fc_test.find_edge_disjoin_route_fast(1, 0, 10, "sw10_vir2442_rand2", false);
    fc_test.find_all_route(2, 20);
    // fc_test.find_edge_disjoin_route_fast(8, 2, 10000, "sw500_vir713137_rand7", true);
    // fc_test.find_edge_disjoin_route_fast(8, 2, 10000, "sw2000_vir51010105_rand2", true);
    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}
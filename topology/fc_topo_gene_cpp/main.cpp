#include <sys/time.h>
#include "fc_edge_disjoin_route.h"

int main(){
    // int switches = 2000;
    // int hosts = 24;
    // int ports = 64;
    // int vir_layer_degree[] = {5, 10, 10, 10, 5};
    // int layer_num = 5;
    // int is_random = 1;
    // int random_seed = 3;

    // int switches = 1000;
    // int hosts = 24;
    // int ports = 64;
    // int vir_layer_degree[] = {7, 13, 13, 7};
    // int layer_num = 4;
    // int is_random = 1;
    // int random_seed = 3;


    int switches = 5;
    int hosts = 24;
    int ports = 33;
    int vir_layer_degree[] = {1, 3, 3, 1};
    int layer_num = 4;
    int is_random = 1;
    int random_seed = 3;

    uint temp[1000];
    Fc_edge_disjoin_route fc_test(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed);
    fc_test.fc_topo_gene();
    fc_test.path_infor_gene();
    // fc_test.display_all_path();
    fc_test.build_search_dic();
    // fc_test.display_dic(2);
    struct timeval start, end;
    gettimeofday(&start, NULL);
    fc_test.extract_route_path(0, 2, 1, temp);
    // bool if_report = true;
    // int report_inter = 10;
    // bool if_store = true;
    // bool store_part = true;
    // fc_test.pthread_for_all_route(1, if_report, report_inter, if_store, store_part);

    // fc_test.find_edge_disjoin_route(1, 0, 10, "sw5_vir1331_rand3", true);
    // fc_test.find_edge_disjoin_route(8, 0, 1000, "sw1000_vir713137_rand3", true);
    // fc_test.find_edge_disjoin_route(8, 1, 5000, "sw2000_vir51010105_rand3", true);
    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    // FILE* ifs = fopen("all_graph_infor/sw1000_vir713137_rand0/sw1000_vir713137_rand00", "r");
    // uint16_t data[1000];
    // fread(data, 2, 1000, ifs);
    // cout << data[4] << endl;
    // fclose(ifs);
    return 0;
}
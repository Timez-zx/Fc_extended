#include <sys/time.h>
#include "fc_edge_disjoin_route.h"

int main(){
    int switches = 2000;
    int hosts = 24;
    int ports = 64;
    int vir_layer_degree[] = {5, 10, 10, 10, 5};
    int layer_num = 5;
    int is_random = 1;
    int random_seed = 3;
    Fc_edge_disjoin_route fc_test(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed);
    fc_test.fc_topo_gene();
    fc_test.path_infor_gene();
    // fc_test.display_all_path();
    fc_test.build_search_dic();
    // fc_test.display_dic(2);

    struct timeval start, end;
    gettimeofday(&start, NULL);
    // bool if_report = true;
    // int report_inter = 30000;
    // bool if_store = true;
    // fc_test.pthread_for_all_route(16, if_report, report_inter, if_store);

    fc_test.find_edge_disjoin_route(16, 2, 5000, "sw2000_vir51010105_rand3");
    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    // FILE* ifs = fopen("all_graph_infor/sw1000_vir713137_rand0/sw1000_vir713137_rand00", "r");
    // uint16_t data[1000];
    // fread(data, 2, 1000, ifs);
    // cout << data[4] << endl;
    // fclose(ifs);
    return 0;
}
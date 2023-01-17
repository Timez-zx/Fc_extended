#include <sys/time.h>
#include "fc_topo_all_route.h"

int main(){
    int switches = 1000;
    int hosts = 24;
    int ports = 64;
    int vir_layer_degree[] = {7, 13, 13, 7};
    int layer_num = 4;
    int is_random = 0;
    int random_seed = 2;
    Fc_topo_all_route fc_test(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed);
    fc_test.fc_topo_gene();
    fc_test.path_infor_gene();
    // fc_test.display_all_path();
    fc_test.build_search_dic();
    // fc_test.display_dic(2);

    struct timeval start, end;
    gettimeofday(&start, NULL);
    bool if_report = true;
    int report_inter = 30000;
    bool if_store = true;
    fc_test.pthread_for_all_route(8, if_report, report_inter, if_store);
    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    // FILE* ifs = fopen("all_graph_infor/sw1000_vir713137_rand0/sw1000_vir713137_rand00", "r");
    // uint16_t data[1000];
    // fread(data, 2, 1000, ifs);
    // cout << data[0] << endl;
    // fclose(ifs);
    return 0;
}
#ifndef FC_TOPO_DISJOIN_
#define FC_TOPO_DISJOIN_

#include "fc_topo_all_route.h"
#include "ortools/graph/min_cost_flow.h"


class Fc_edge_disjoin_route: public Fc_topo_all_route{
    public:
        Fc_edge_disjoin_route(int switches, int hosts, int ports, int* vir_layer_degree, int layer_num, int is_random, int random_seed):
        Fc_topo_all_route(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed){}
        ~Fc_edge_disjoin_route(){};

        void find_all_route(int thread_num, int batch_num);
        void find_edge_disjoin_route_fast(int thread_num ,int thread_label, int batch_num, string read_file, bool store_part);
        int verify_route(int src, int dst);
        int dfs(int* head, int* visited, Edge* edge, int src, int dst, uint16_t* store_path);
        
};      

#endif
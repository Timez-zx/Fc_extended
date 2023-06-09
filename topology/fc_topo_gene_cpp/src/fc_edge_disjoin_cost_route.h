#ifndef FC_TOPO_DISJOIN_COST_
#define FC_TOPO_DISJOIN_COST_

#include <mutex>
#include "fc_topo_all_route.h"
#include "ortools/graph/min_cost_flow.h"

/*
Fc_edge_disjoin_cost_route: Set the cost of the edges differently based on the usage of the edges
when solving the edge-disjoint paths
However, the problems of edge-disjoint or up-down are the poor path diversity, it is useless because it can't
solve the principal controdiction
*/

class Fc_edge_disjoin_cost_route: public Fc_topo_all_route{
    public:
        Fc_edge_disjoin_cost_route(int switches, int hosts, int ports, int* vir_layer_degree, int layer_num, int is_random, int random_seed):
        Fc_topo_all_route(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed){}
        ~Fc_edge_disjoin_cost_route();
        void fc_topo_gene(void);
        void fc_topo_gene_1v1(int fast_or_not);
        void find_all_route(int thread_num, int batch_num);
        void find_edge_disjoin_route_fast(int thread_num ,int thread_label, int batch_num, string read_file, bool store_part);
        int dfs(int* head, int* visited, Edge* edge, int src, int dst, uint16_t* store_path);

    private:
        unordered_map<int,int> *cost_map;
        mutex *mtx;

};      

#endif
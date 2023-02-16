#ifndef FC_TOPO_ALL_
#define FC_TOPO_ALL_

#include "fc_base.h"


const int MAX_NUM = 10000;

struct Edge
{
    int next;
    int to;
    int weight;
};

typedef struct {   
    int** path_infor;
    int* path_len;
    int path_num;
} node_path_infor;

typedef struct {
    int* index_table;
    vector<int> participate;
    vector<vector<int> > top_path_label;
    vector<vector<int> > top_loc_label;
} topo_dic_infor;


class Fc_topo_all_route: public Fc_base{
    public:
        Fc_topo_all_route(int switches, int hosts, int ports, int* vir_layer_degree, int layer_num, int is_random, int random_seed):
        Fc_base(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed){}

        ~Fc_topo_all_route();
        // Generate topology, but a pair of switches may have multi links.
        int  change_base(int basic);
        void fc_topo_gene(void);

        // Search and save all up-down paths from beginning from the top layer
        void path_infor_gene(void);
        void search_path(int root, node_path_infor* node_infor);
        void display_all_path(void);
        // Build the dic to search a node location in paths of other nodes
        void build_search_dic(void);
        void display_dic(int index);

        // Search for all the sub-graphs which are constructed by the up-down paths from all src to dst
        void pthread_for_all_route(int thread_num, bool if_report, int report_inter, bool if_store, bool store_part);
        uint extract_route_path(int src, int dst, bool if_display, uint* return_graph);
        void thread_route(vector<int*> route_pairs, int thread_label, bool if_report, int report_inter, bool if_store, string store_file, bool store_part);

        // Search for all the up-down paths from all src to dst
        // Especially, the "ksp" mode is used to find the k shortest paths in all the up-down paths
        void pthread_for_all_path(int thread_num, bool if_report, int report_inter, bool if_store, string mode, int ksp_num);
        uint16_t extract_all_path(int src, int dst, bool if_display, string mode, int ksp_num, uint16_t* return_path, uint16_t *path_num);
        void thread_all_path(vector<int*> route_pairs, int thread_label, bool if_report, int report_inter, bool if_store, string store_file, string mode, int ksp_num);
        
    protected:
        node_path_infor* all_path_infor=NULL;
        topo_dic_infor* topo_dic=NULL;
};

#endif
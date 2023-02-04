#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <queue>
#include <cmath>
#include <vector>
#include <thread>
#include <random>
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include <string>
#include "gurobi_c++.h"
using namespace std;

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


class Fc_topo_all_route{
    public:
        // Initial part
        int switches;
        int hosts;
        int ports;
        int* vir_layer_degree;
        int layer_num;
        int is_random;
        int random_seed;

        int* bipart_degree=NULL;
        int* topo_index=NULL;
        int **bit_map;

        node_path_infor* all_path_infor=NULL;
        topo_dic_infor* topo_dic=NULL;

        Fc_topo_all_route(int switches, int hosts, int ports, int* vir_layer_degree, int layer_num, int is_random, int random_seed):
        switches(switches), hosts(hosts), ports(ports), vir_layer_degree(vir_layer_degree), layer_num(layer_num),is_random(is_random), random_seed(random_seed){}

        ~Fc_topo_all_route();
        // generate topology
        int  change_base(int basic);
        void fc_topo_gene(void);
        void fc_topo_gene_1v1(void);
        void search_path(int root, node_path_infor* node_infor);
        void path_infor_gene(void);
        void display_all_path(void);
        void build_search_dic(void);
        void display_dic(int index);
        uint extract_route_path(int src, int dst, bool if_display, uint* return_graph);
        void thread_route(vector<int*> route_pairs, int thread_label, bool if_report, int report_inter, bool if_store, string store_file, bool store_part);
        void pthread_for_all_route(int thread_num, bool if_report, int report_inter, bool if_store, bool store_part);

        uint16_t extract_all_path(int src, int dst, bool if_display, uint16_t* return_path, uint16_t *path_num);
        void thread_all_path(vector<int*> route_pairs, int thread_label, bool if_report, int report_inter, bool if_store, string store_file);
        void pthread_for_all_path(int thread_num, bool if_report, int report_inter, bool if_store);
        void throughput_test(string type, int seed);
        void gene_uniform_random(float **flow_matrix, int seed);
        void gene_worse_case(float **flow_matrix);
};
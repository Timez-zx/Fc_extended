#ifndef FC_TAGGER_KSPZ_
#define FC_TAGGER_KSPZ_

#include <map>
#include "fc_base.h"
#include "KSP_PROZ/shortest_path.h"

/*
Fc_tagger_ksp: select the k shortest paths whose down-up times are smaller than (the lossless queues - 1)
we can use, the number of lossless queue is named by vc_num. 
*/

class Fc_tagger_kspz: public Fc_base{
    public:
        Fc_tagger_kspz(int switches, int hosts, int ports, int* vir_layer_degree, int layer_num, int is_random, int random_seed):
        Fc_base(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed){}
        ~Fc_tagger_kspz(){};
        
        // Save the topo infor in files for ksp function interface 
        void save_graph_infor();

        string gene_path_for_file_ksp(string path, int ksp_num, int vc_num);

        // Use multi threads to calculate ksp-paths with small down-up times
        // path_num: the number of shortest paths vc_num: the number of lossless queues we can use
        void pthread_up_down_ksp(int thread_num, int path_num, int vc_num, bool if_report, int report_inter, bool if_store);
        uint16_t search_up_down_ksp(int src, int dst, int path_num, int vc_num, uint16_t *path_infor, int thread_label);
        void thread_up_down_ksp(vector<int*> route_pairs, int thread_label, int path_num, int vc_num, bool if_report, int report_inter, bool if_store, string store_file);

        // redefine the throughput because of different parameters
        double throughput_test_ksp(string type, int seed, int path_num, int vc_num);

    private:
        unordered_map<int,int> link_to_layer_map;
        unordered_map<int,int> link_to_port_map;
        string topo_path;
        Graph **graph_pr=NULL;
        int *thread_tag_num=NULL;
        int max_tag_num;
        double *thread_tag_kspN=NULL;
        double ave_tag_kspN;
};

#endif
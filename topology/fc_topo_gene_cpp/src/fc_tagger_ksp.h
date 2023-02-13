#ifndef FC_TAGGER_KSP_
#define FC_TAGGER_KSP_

#include <map>
#include "fc_base.h"
#include "KSP/GraphElements.h"
#include "KSP/Graph.h"
#include "KSP/DijkstraShortestPathAlg.h"
#include "KSP/YenTopKShortestPathsAlg.h"

class Fc_tagger_ksp: public Fc_base{
    public:
        Fc_tagger_ksp(int switches, int hosts, int ports, int* vir_layer_degree, int layer_num, int is_random, int random_seed):
        Fc_base(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed){}
        ~Fc_tagger_ksp(){};
        void save_graph_infor();
        void search_up_down_ksp(int src, int dst, int path_num, int vc_num);
        
    
    private:
        unordered_map<int,int> link_to_layer_map;
        Graph *my_graph;
};

#endif
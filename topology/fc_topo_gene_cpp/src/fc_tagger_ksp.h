#ifndef FC_TAGGER_KSP_
#define FC_TAGGER_KSP_


#include "fc_base.h"

class Fc_tagger_ksp: public Fc_base{
    public:
        Fc_tagger_ksp(int switches, int hosts, int ports, int* vir_layer_degree, int layer_num, int is_random, int random_seed):
        Fc_base(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed){}
        ~Fc_tagger_ksp(){};
        void save_graph_infor();
    
    private:
        unordered_map<int,int> link_to_layer_map;




};

#endif
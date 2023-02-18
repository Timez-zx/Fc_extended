#ifndef FC_TAGGER_TEST_
#define FC_TAGGER_TEST_

#include <map>
#include "fc_base.h"
#include "KSP_PROZ/shortest_path.h"

/*
Fc_tagger_ksp: select the k shortest paths whose down-up times are smaller than (the lossless queues - 1)
we can use, the number of lossless queue is named by vc_num. 
*/

class FcTaggerTest: public Fc_base{
    public:
        FcTaggerTest(int switches, int hosts, int ports, int* vir_layer_degree, int layer_num, int is_random, int random_seed):
        Fc_base(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed){}
        ~FcTaggerTest(){};
        
        // Save the topo infor in files for ksp function interface 
        void SaveTaggerGraph();


    private:
        unordered_map<int,int> linkPortMap;
        string topoPath;
        Graph **graphPr=NULL;
};

#endif
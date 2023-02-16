#ifndef FC_BASE_
#define FC_BASE_

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

/*
Fc_base: base class for some evaluation function
*/

class Fc_base{
    public:
        // Initial part
        int switches;
        int hosts;
        int ports;
        int* vir_layer_degree;
        int layer_num;
        int is_random;
        int random_seed;

        Fc_base(int switches, int hosts, int ports, int* vir_layer_degree, int layer_num, int is_random, int random_seed):
        switches(switches), hosts(hosts), ports(ports), vir_layer_degree(vir_layer_degree), layer_num(layer_num),is_random(is_random), random_seed(random_seed){
            if(access("data", 0)){
                string cmd("mkdir ");
                cmd += "data";
                int temp = system(cmd.c_str());
            }
        }

        ~Fc_base();
        // two mode, fast mode can build fast but has high prob to fail
        void fc_topo_gene_1v1(int fast_or_not);

        string gene_path_for_file(string path);

        // three mode: seed is to generate the uniform random flow metrix
        // all to all - "aa", uniform random - "ur", near-worst - "wr" 
        double throughput_test(string type, int seed);
        void gene_uniform_random(float **flow_matrix, int seed);
        void gene_worse_case(float **flow_matrix);

        // Evaluate the length of fiber and the number of tranceivers
        // distance_infor: [0] Tor to tor:x [1] Tor to tor:y [2] Ocs to tor:x [3] Ocs to ocs:y
        // column_num: the number of tors for a column
        void cost_model(int ocs_ports, int* distance_infor, int column_num);

        // Use the heuristic to calculate the bisection bandwidth
        // rand_interval: the number of random seeds to decide the initial status
        // cycle_times: max times at the local mini
        // poss_base: give possi to jump out local mini, poss_base is smaller, the poss is larger
        void multi_thread_b_bandwidth(int thread_num, int rand_interval, int cycle_times, int poss_base);
        int bisection_bandwidth_byExchange(int random_seed, int cycle_times, int poss_base);
        void b_bandwidth_onethread(int thread_label, int rand_interval, int cycle_times, int* band_find, int poss_base);

    protected:
        int* bipart_degree=NULL;
        int* topo_index=NULL;
        int** bit_map=NULL;
};

#endif
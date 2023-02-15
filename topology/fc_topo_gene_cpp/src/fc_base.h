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

        double throughput_test(string type, int seed);
        void gene_uniform_random(float **flow_matrix, int seed);
        void gene_worse_case(float **flow_matrix);

        void cost_model(int ocs_ports, int* distance_infor, int column_num);
        int bisection_bandwidth_byExchange(int random_seed, int cycle_times, int poss_base);
        void b_bandwidth_onethread(int thread_label, int rand_interval, int cycle_times, int* band_find, int poss_base);
        void multi_thread_b_bandwidth(int thread_num, int rand_interval, int cycle_times, int poss_base);

    protected:
        int* bipart_degree=NULL;
        int* topo_index=NULL;
        int** bit_map=NULL;
};

#endif
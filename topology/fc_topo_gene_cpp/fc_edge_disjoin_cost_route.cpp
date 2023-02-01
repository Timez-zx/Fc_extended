#include "fc_edge_disjoin_cost_route.h"

Fc_edge_disjoin_cost_route::~Fc_edge_disjoin_cost_route(){
    if(cost_map){
        delete[] cost_map;
        cost_map = NULL;
    }
}

void Fc_edge_disjoin_cost_route::fc_topo_gene(void){
    int sw_ports = ports - hosts;
    int bipart_num = layer_num - 1;
    int* switch_array = new int[switches];
    int degree, basic;
    int* degrees = new int[switches];
    queue<int> remains;
    int src, dst;
    int initial_sub = 0;
    bipart_degree = new int[bipart_num+1];
    cost_map = new unordered_map<int,int>[switches];
    int bipart_count = 0;

    int total_degree = 0;
    for(int i = 0; i < switches; i++){
        switch_array[i] = i;
    }
    int remain_degree = vir_layer_degree[layer_num-1];
  
    total_degree += remain_degree + 1;
    for(int i = layer_num-2; i > 0; i--){
        remain_degree = vir_layer_degree[i] - remain_degree;
        total_degree += remain_degree + 1;
    }
    topo_index = new int[switches*total_degree];

    int index_basic = 0;
    int node1, node2;
    for(int i = layer_num - 1; i > 0; i--){
        degree = vir_layer_degree[i] - initial_sub;
        bipart_degree[bipart_count] = degree + 1;
        bipart_count++;
        for(int j = 0; j < switches; j++){
            degrees[j] = degree;
        }
        basic = switches - 1;
        if(is_random){
            srand(random_seed);
            shuffle(switch_array, switch_array+switches, default_random_engine(random_seed));
        }
        for(int j = 0; j < switches; j++){
            src = switch_array[j];
            topo_index[index_basic+src*(degree+1)] = src;
            for(int k = 0; k < degree; k++){
                if(basic == j){
                    remains.push(j);
                    basic = change_base(basic);
                }
                dst = switch_array[basic];
                while(degrees[dst] == 0){
                    basic = change_base(basic);
                    dst = switch_array[basic];
                }
                if(remains.size() > 0){
                    if(remains.front() != j){
                        dst = switch_array[remains.front()];
                        basic = remains.front();
                        remains.pop();
                        basic = change_base(basic);
                    }
                    else
                        basic = change_base(basic);
                }
                else
                    basic = change_base(basic);
                degrees[dst]--;
                topo_index[index_basic+src*(degree+1)+k+1] = dst;
                node1 = ((layer_num-1-i)*2+i)*switches+src;
                node2 = ((layer_num-i)*2+i-1)*switches+dst;
                cost_map[src][node1*switches*(2*layer_num-1)+node2] = 1;
                node1 = (i-1)*switches+dst;
                node2 = i*switches+src;
                cost_map[dst][node1*switches*(2*layer_num-1)+node2] = 1;
                // cout << src << "->" << dst << endl;
            }
        }
        // cout << endl;
        initial_sub = degree;
        index_basic += (degree+1)*switches;
    }
    bipart_degree[bipart_count] = 0;
    delete[] switch_array;
    delete[] degrees;
}

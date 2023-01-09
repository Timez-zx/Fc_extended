#include <iostream>
#include <algorithm>
#include <queue>
#include <cmath>
using namespace std;

int Rand(int i){return rand()%i;}

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

        int* bipart_degree;
        int* topo_index;

        Fc_topo_all_route(int switches, int hosts, int ports, int* vir_layer_degree, int layer_num, int is_random, int random_seed):
        switches(switches), hosts(hosts), ports(ports), vir_layer_degree(vir_layer_degree), layer_num(layer_num),is_random(is_random), random_seed(random_seed){}
        // generate topology
        int  change_base(int basic);
        void fc_topo_gene(void);
        void search_path(int root);
};

int Fc_topo_all_route::change_base(int basic){
    int new_basic;
    if(basic == 0)
        new_basic = switches - 1;
    else
        new_basic = basic - 1;
    return new_basic;
}

void Fc_topo_all_route::fc_topo_gene(void){
    int sw_ports = ports - hosts;
    int bipart_num = layer_num - 1;
    int* switch_array = new int[switches];
    int degree, basic;
    int* degrees = new int[switches];
    queue<int> remains;
    int src, dst;
    int initial_sub = 0;
    bipart_degree = new int[bipart_num+1];
    int bipart_count = 0;

    int total_degree = 0;
    for(int i = 0; i < switches; i++){
        switch_array[i] = i;
    }
    for(int i = 0; i < layer_num; i++){
        total_degree += vir_layer_degree[i];
    }
    topo_index = new int[switches*total_degree/2];
    int topo_count = 0;

    for(int i = layer_num - 1; i > 0; i--){
        degree = vir_layer_degree[i] - initial_sub;
        bipart_degree[bipart_count] = degree;
        bipart_count++;
        for(int j = 0; j < switches; j++){
            degrees[j] = degree;
        }
        basic = switches - 1;
        if(is_random){
            srand(random_seed);
            random_shuffle(switch_array, switch_array+switches-1, Rand);
        }
        for(int j = 0; j < switches; j++){
            src = switch_array[j];
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
                topo_index[topo_count] = dst;
                topo_count++;
            }
        }
        initial_sub = degree;
    }
    bipart_degree[bipart_count] = 0;
    delete[] switch_array;
    delete[] degrees;
}

void Fc_topo_all_route::search_path(int root){
    int max_path = pow(2, layer_num-1);
    int** root_path = new int*[max_path];
    int* path_len = new int[max_path + 1];
    int layer_root, layer_degree, temp_path_num;
    int basic = 0;
    int node, flag, drop_num = 0;

    path_len[max_path] = 1;
    for(int i = 0; i < max_path; i++){
        root_path[i] = new int[layer_num];
    }
    root_path[0][0] = root;
    path_len[0] = 1;

    for(int i = 0; i < layer_num - 1; i++){
        layer_degree = bipart_degree[i];
        temp_path_num = path_len[max_path];
        for(int j = 0; j < temp_path_num; j++){
            layer_root = root_path[j][i];
            for(int k = 0; k < layer_degree; k++){
                node = topo_index[basic + layer_degree*layer_root + k];
                cout << layer_root << " " << node << endl;
                if((i != layer_num - 2) | (node != layer_root)){
                    flag = 0;
                    for(int len = 0; len < path_len[j]; len++){
                        if(root_path[j][len] == node)
                            flag = 1;
                    }
                    if((!flag) | (node == layer_root)){
                        if(k == 0){
                            root_path[j][i+1] = node;
                            path_len[j]++;
                        }
                        else{
                            memcpy(root_path[path_len[max_path]], root_path[j], 4*layer_num);
                            root_path[path_len[max_path]][i+1] = node;
                            path_len[path_len[max_path]] = path_len[j] + 1;
                            path_len[max_path]++;
                        }
                    }
                }
            }
        }
        basic += layer_degree*switches;
    }
    cout << path_len[max_path] << endl;
}

int main(){
    int switches = 5;
    int hosts = 24;
    int ports = 36;
    int vir_layer_degree[] = {2, 4, 4, 2};
    int layer_num = 4;
    int is_random = 1;
    int random_seed = 1;
    Fc_topo_all_route fc_test(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed);
    fc_test.fc_topo_gene();
    fc_test.search_path(0);
    return 0;
}
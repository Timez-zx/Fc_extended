#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <cmath>
#include <vector>
using namespace std;

typedef struct {   
    int** path_infor;
    int* path_len;
    int path_num;
} node_path_infor;

typedef struct {
    int participate_num;
    int* index_table;
    vector<int> top_loc_len;
    vector<vector<int> > top_path_label;
    vector<vector<int> > top_loc_label;
} topo_dic_infor;

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

        node_path_infor* all_path_infor;
        topo_dic_infor* topo_dic;

        Fc_topo_all_route(int switches, int hosts, int ports, int* vir_layer_degree, int layer_num, int is_random, int random_seed):
        switches(switches), hosts(hosts), ports(ports), vir_layer_degree(vir_layer_degree), layer_num(layer_num),is_random(is_random), random_seed(random_seed){}
        // generate topology
        int  change_base(int basic);
        void fc_topo_gene(void);
        void search_path(int root, node_path_infor* node_infor);
        void path_infor_gene(void);
        void display_all_path(void);
        void build_search_dic(void);
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
    int remain_degree = vir_layer_degree[layer_num-1];
    total_degree += remain_degree + 1;
    for(int i = layer_num-2; i > 0; i--){
        remain_degree = vir_layer_degree[i] - remain_degree;
        total_degree += remain_degree + 1;
    }
    topo_index = new int[switches*total_degree];
    int topo_count = 0;

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
            random_shuffle(switch_array, switch_array+switches-1, Rand);
        }
        for(int j = 0; j < switches; j++){
            src = switch_array[j];
            topo_index[topo_count] = src;
            topo_count++;
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

void Fc_topo_all_route::search_path(int root, node_path_infor* node_infor){
    int layer_root, layer_degree, temp_path_num;
    int basic = 0;
    int node, flag;

    node_infor->path_num = 1;
    node_infor->path_infor[0][0] = root;
    node_infor->path_len[0] = 1;
    for(int i = 0; i < layer_num - 1; i++){
        layer_degree = bipart_degree[i];
        temp_path_num = node_infor->path_num;
        for(int j = 0; j < temp_path_num; j++){
            layer_root = node_infor->path_infor[j][i];
            for(int k = 0; k < layer_degree; k++){
                node = topo_index[basic + layer_degree*layer_root + k];
                if((i != layer_num - 2) | (node != layer_root)){
                    flag = 0;
                    for(int len = 0; len < node_infor->path_len[j]; len++){
                        if(node_infor->path_infor[j][len] == node)
                            flag = 1;
                    }
                    if((!flag) | (node == layer_root)){
                        if(node_infor->path_len[j] == i + 1){
                            node_infor->path_infor[j][i+1] = node;
                            node_infor->path_len[j]++;
                        }
                        else{
                            memcpy(node_infor->path_infor[node_infor->path_num], node_infor->path_infor[j], 4*(i+1));
                            node_infor->path_infor[node_infor->path_num][i+1] = node;
                            node_infor->path_len[node_infor->path_num] = (i+1) + 1;
                            node_infor->path_num++;
                        }
                    }
                }
            }
        }
        basic += layer_degree*switches;
    }
}

void Fc_topo_all_route::path_infor_gene(void){
    int remain_degree = vir_layer_degree[layer_num-1];
    int max_path = 1;
    max_path *= remain_degree + 1;
    for(int i = layer_num-2; i > 0; i--){
        remain_degree = vir_layer_degree[i] - remain_degree;
        max_path *= remain_degree + 1;
    }
    node_path_infor node_infor;
    node_infor.path_len = new int[max_path];
    node_infor.path_infor = new int*[max_path];
    for(int i = 0; i < max_path; i++){
        node_infor.path_infor[i] = new int[layer_num];
    }
    all_path_infor = new node_path_infor[switches];
    for(int i = 0; i < switches; i++){
        search_path(i, &node_infor);
        all_path_infor[i].path_infor = new int*[node_infor.path_num];
        all_path_infor[i].path_len = new int[node_infor.path_num];
        all_path_infor[i].path_num = node_infor.path_num;
        for(int j = 0; j < node_infor.path_num; j++){
            all_path_infor[i].path_len[j] = node_infor.path_len[j];
            all_path_infor[i].path_infor[j] = new int[node_infor.path_len[j]];
            for(int k = 0; k < node_infor.path_len[j]; k++){
                all_path_infor[i].path_infor[j][k] = node_infor.path_infor[j][k];
            }
        }
    }
    delete[] node_infor.path_len;
    for(int i = 0; i < max_path; i++){
        delete[] node_infor.path_infor[i];
    }
    delete[] node_infor.path_infor;
}

void Fc_topo_all_route::display_all_path(void){
    for(int i = 0; i < switches; i++){
        cout << all_path_infor[i].path_num << endl;
        for(int j = 0; j < all_path_infor[i].path_num; j++){
            for(int k = 0; k < all_path_infor[i].path_len[j]; k++){
                cout << all_path_infor[i].path_infor[j][k] << " ";
            }
            cout << endl;
        }
    }
}

void Fc_topo_all_route::build_search_dic(void){
    topo_dic = new topo_dic_infor[switches];
    for(int i = 0; i < switches; i++){
        topo_dic[i].index_table = new int[switches];
        memset(topo_dic[i].index_table, 0xff, switches*4);
        topo_dic[i].participate_num = 0;
    }

    for(int i = 0; i < switches; i++){
        for(int j = 0; j < all_path_infor[i].path_num; j++){
            for(int k = 0; k < all_path_infor[i].path_len[j]; k++){
                int node = all_path_infor[i].path_infor[j][k];
                if(node != i){
                    if(topo_dic[node].index_table[i] == -1){
                        topo_dic[node].index_table[i] = topo_dic[node].participate_num;
                        vector<int> path_label;
                        vector<int> loc_label;
                        path_label.push_back(j);
                        topo_dic[node].top_path_label.push_back(path_label);
                        loc_label.push_back(k);
                        topo_dic[node].top_loc_label.push_back(loc_label);
                        topo_dic[node].top_loc_len.push_back(1);
                        topo_dic[node].participate_num++;
                    }
                    else{
                        int temp_index = topo_dic[node].index_table[i];
                        topo_dic[node].top_path_label[temp_index].push_back(j);
                        topo_dic[node].top_loc_label[temp_index].push_back(k);
                        topo_dic[node].top_loc_len[temp_index]++;
                    }
                }
            }
        }
    }

}



int main(){
    int switches = 5;
    int hosts = 24;
    int ports = 36;
    int vir_layer_degree[] = {2, 4, 4, 2};
    int layer_num = 4;
    int is_random = 0;
    int random_seed = 1;
    Fc_topo_all_route fc_test(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed);
    fc_test.fc_topo_gene();
    fc_test.path_infor_gene();
    fc_test.build_search_dic();
    return 0;
}
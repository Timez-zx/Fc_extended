#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <set>
#include <queue>
#include <cmath>
#include <vector>
#include <sys/time.h>
#include <thread>
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include <string>
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

        ~Fc_topo_all_route();
        // generate topology
        int  change_base(int basic);
        void fc_topo_gene(void);
        void search_path(int root, node_path_infor* node_infor);
        void path_infor_gene(void);
        void display_all_path(void);
        void build_search_dic(void);
        void display_dic(int index);
        vector<vector<int> > extract_route_path(int src, int dst, bool if_display);
        void thread_route(vector<int*> route_pairs, int thread_label, bool if_report, int report_inter, bool if_store, int store_num, string store_file);
        void pthread_for_all_route(int thread_num, bool if_report, int report_inter, bool if_store, int store_num);
};

Fc_topo_all_route::~Fc_topo_all_route(){
    if(bipart_degree){
        delete[] bipart_degree;
        bipart_degree = NULL;
    }
    if(topo_index){
        delete[] topo_index;
        topo_index = NULL;
    }
    if(all_path_infor){
        for(int i = 0; i < switches; i++){
            for(int j = 0; j < all_path_infor[i].path_num; j++){
                if(all_path_infor[i].path_infor[j]){
                    delete[] all_path_infor[i].path_infor[j];
                    all_path_infor[i].path_infor[j] = NULL;
                }
            }
            if(all_path_infor[i].path_len){
                delete[] all_path_infor[i].path_len;
                all_path_infor[i].path_len = NULL;
            }
        }
        delete[] all_path_infor;
        all_path_infor = NULL;
    }
    if (topo_dic)
    {
        for(int i = 0; i < switches; i++){
            if(topo_dic[i].index_table){
                delete[] topo_dic[i].index_table;
                topo_dic[i].index_table = NULL;
            }
        }   
        delete[] topo_dic;
        topo_dic = NULL;
    }
    
}

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
    }

    for(int i = 0; i < switches; i++){
        for(int j = 0; j < all_path_infor[i].path_num; j++){
            for(int k = 0; k < all_path_infor[i].path_len[j]; k++){
                int node = all_path_infor[i].path_infor[j][k];
                if(node != i){
                    if(topo_dic[node].index_table[i] == -1){
                        topo_dic[node].index_table[i] = topo_dic[node].participate.size();
                        topo_dic[node].participate.push_back(i);
                        vector<int> path_label;
                        path_label.clear();
                        vector<int> loc_label;
                        loc_label.clear();
                        path_label.push_back(j);
                        topo_dic[node].top_path_label.push_back(path_label);
                        loc_label.push_back(k);
                        topo_dic[node].top_loc_label.push_back(loc_label);
                    }
                    else{
                        int temp_index = topo_dic[node].index_table[i];
                        int len = topo_dic[node].top_loc_label[temp_index].size();
                        if(j != topo_dic[node].top_path_label[temp_index][len-1]){
                            topo_dic[node].top_path_label[temp_index].push_back(j);
                            topo_dic[node].top_loc_label[temp_index].push_back(k);
                        }
                    }
                }
            }
        }
    }
}

void Fc_topo_all_route::display_dic(int index){
    for (int i = 0; i < topo_dic[index].participate.size(); i++)
    {
        cout << topo_dic[index].participate[i] << endl;
        for(int j = 0; j <  topo_dic[index].top_loc_label[i].size(); j++){
            cout << topo_dic[index].top_path_label[i][j] << "-" << topo_dic[index].top_loc_label[i][j] << " ";
        }
        cout << endl;
    }
}

vector<vector<int> > Fc_topo_all_route::extract_route_path(int src, int dst, bool if_display){
    vector<vector<int> > route_node_path;
    set<vector<int> > temp_node_path;
    vector<vector<int> > temp_vec;

    int temp_index_src = topo_dic[dst].index_table[src];
    vector<int> path_label;
    vector<int> loc_label;
    vector<int> node_path;
    if(temp_index_src >= 0){
        path_label = topo_dic[dst].top_path_label[temp_index_src];
        loc_label = topo_dic[dst].top_loc_label[temp_index_src];
        for(int i = 0; i < path_label.size(); i++){
            int *path = all_path_infor[src].path_infor[path_label[i]];
            int loc = loc_label[i];
            node_path.push_back(path[loc]);
            node_path.push_back(layer_num - loc);
            loc--;
            while(path[loc] != src){
                node_path.push_back(path[loc]);
                node_path.push_back(layer_num - loc);
                loc--;
            }
            node_path.push_back(path[loc]);
            node_path.push_back(layer_num - loc);
            temp_node_path.insert(node_path);
            node_path.clear();
        }
        temp_vec.assign(temp_node_path.begin(), temp_node_path.end());
        route_node_path.insert(route_node_path.end(), temp_vec.begin(), temp_vec.end());
        temp_node_path.clear();
        temp_vec.clear();
    }

    int temp_index_dst = topo_dic[src].index_table[dst];
    if(temp_index_dst >= 0){
        path_label = topo_dic[src].top_path_label[temp_index_dst];
        loc_label = topo_dic[src].top_loc_label[temp_index_dst];
        for(int i = 0; i < path_label.size(); i++){
            int *path = all_path_infor[dst].path_infor[path_label[i]];
            int loc = loc_label[i];
            node_path.push_back(path[loc]);
            node_path.push_back(layer_num - loc);
            loc--;
            while(path[loc] != dst){
                node_path.push_back(path[loc]);
                node_path.push_back(layer_num - loc);
                loc--;
            }
            node_path.push_back(path[loc]);
            node_path.push_back(layer_num - loc);
            temp_node_path.insert(node_path);
            node_path.clear();
        }
        temp_vec.assign(temp_node_path.begin(), temp_node_path.end());
        route_node_path.insert(route_node_path.end(), temp_vec.begin(), temp_vec.end());
        temp_node_path.clear();
        temp_vec.clear();
    }

    vector<int> src_part = topo_dic[src].participate;
    vector<int> dst_part = topo_dic[dst].participate;
    vector<int> inter;
    set_intersection(src_part.begin(), src_part.end(), dst_part.begin(), dst_part.end(), inserter(inter, inter.begin()));


    vector<int> src_inter_path;
    vector<int> src_inter_loc;
    vector<int> dst_inter_path;
    vector<int> dst_inter_loc;

    set<vector<int> > src_valid_set;
    set<vector<int> > dst_valid_set;

    vector<vector<int> > src_valid_path;
    vector<vector<int> > dst_valid_path;

    vector<int> temp_path(layer_num+1);
    int* temp_array;

    vector<int> src_path, dst_path;
    for(int i = 0; i < inter.size(); i++){
        temp_index_src = topo_dic[src].index_table[inter[i]];
        src_inter_path = topo_dic[src].top_path_label[temp_index_src];
        src_inter_loc = topo_dic[src].top_loc_label[temp_index_src];
        temp_index_dst = topo_dic[dst].index_table[inter[i]];
        dst_inter_path = topo_dic[dst].top_path_label[temp_index_dst];
        dst_inter_loc = topo_dic[dst].top_loc_label[temp_index_dst];
        for(int j = 0; j < src_inter_path.size(); j++){
            temp_array = all_path_infor[inter[i]].path_infor[src_inter_path[j]];
            memcpy(&temp_path[0], temp_array, (src_inter_loc[j] + 1)*4);
            memset(&temp_path[src_inter_loc[j]+1], 0, (layer_num - 1 - src_inter_loc[j])*4);
            temp_path[layer_num] = src_inter_loc[j];
            if(find(&temp_path[0], &temp_path[src_inter_loc[j] + 1], dst) == &temp_path[src_inter_loc[j] + 1])
                src_valid_set.insert(temp_path);
        }
        src_valid_path.assign(src_valid_set.begin(), src_valid_set.end());

        for(int j = 0; j < dst_inter_path.size(); j++){
            temp_array = all_path_infor[inter[i]].path_infor[dst_inter_path[j]];
            memcpy(&temp_path[0], temp_array, (dst_inter_loc[j] + 1)*4);
            memset(&temp_path[dst_inter_loc[j]+1], 0, (layer_num - 1 - dst_inter_loc[j])*4);
            temp_path[layer_num] = dst_inter_loc[j];
            if(find(&temp_path[0], &temp_path[dst_inter_loc[j] + 1], src) == &temp_path[dst_inter_loc[j] + 1])
                dst_valid_set.insert(temp_path);
        }
        dst_valid_path.assign(dst_valid_set.begin(), dst_valid_set.end());
        for(int src = 0; src < src_valid_path.size(); src++){
            for(int dst = 0; dst < dst_valid_path.size(); dst++){
                src_path = src_valid_path[src];
                dst_path = dst_valid_path[dst];
                int begin_src = 0;
                int begin_dst = 0;

                while(src_path[begin_src] == dst_path[begin_dst]){
                    if(src_path[begin_src] == inter[i]){
                        begin_src++;
                        begin_dst++;
                    }
                    else
                        break;
                }
                begin_src--;
                vector<int> pass_node;
                int pass_flag = 0;
                for(int k = src_path[layer_num]; k >= begin_src; k--){
                    node_path.push_back(src_path[k]);
                    pass_node.push_back(src_path[k]);
                    node_path.push_back(layer_num - k);
                }
                for(int k = begin_dst; k <= dst_path[layer_num]; k++){
                    if(find(pass_node.begin(), pass_node.end(), dst_path[k]) != pass_node.end()){
                        pass_flag = 1;
                        node_path.clear(); 
                        break;
                    }
                    node_path.push_back(dst_path[k]);
                    node_path.push_back(layer_num - k);
                }
                if(pass_flag)
                    continue;
                temp_node_path.insert(node_path);
                node_path.clear(); 
                
            }
        }
        src_valid_set.clear();
        src_valid_path.clear();
        dst_valid_path.clear();
        dst_valid_set.clear();
        temp_vec.assign(temp_node_path.begin(), temp_node_path.end());
        route_node_path.insert(route_node_path.end(), temp_vec.begin(), temp_vec.end());
        temp_node_path.clear();
        temp_vec.clear();
    }
    if(if_display){
        cout << route_node_path.size() << endl;
        int count = 0;
        for(int i = 0; i < route_node_path.size(); i++){
            for(int j = 0; j < route_node_path[i].size(); j++){
                cout << route_node_path[i][j] << " ";
                count++;
            }
            cout << endl;
        }
        cout<< count <<endl;
    }

    set<vector<int> > graph_infor;
    vector<vector<int> > graph_infor_vec;
    for(int i = 0; i < route_node_path.size(); i++){
        for(int j = 0; j < route_node_path[i].size()-3; j += 2){
            vector<int> temp(4);
            temp[0] = route_node_path[i][j];
            temp[1] = route_node_path[i][j+1];
            temp[2] = route_node_path[i][j+2];
            temp[3] = route_node_path[i][j+3];
            graph_infor.insert(temp);
        }
    }
    graph_infor_vec.assign(graph_infor.begin(), graph_infor.end());
    return graph_infor_vec;
}

void Fc_topo_all_route::thread_route(vector<int*> route_pairs, int thread_label, bool if_report, int report_inter, bool if_store, int store_num, string store_file) {
    int count = 0;
    if(if_store){
        vector<vector<vector<int> > > store_graph_info(store_num);
    }
    for(int i = 0; i < route_pairs.size(); i++){
        vector<vector<int> > temp_infor = extract_route_path(route_pairs[i][0], route_pairs[i][1], false);
        if(if_report){
            count++;
            if(count % report_inter == 0){
                cout << "The thread " << thread_label << " " <<count/double(route_pairs.size()) << endl;
            }
        }
    }
}

void Fc_topo_all_route::pthread_for_all_route(int thread_num, bool if_report, int report_inter, bool if_store, int store_num){
    int total_pairs = switches*(switches-1)/2;
    int average = ceil(total_pairs/thread_num);
    int count = 0;
    int allo_index = 0;
    vector<vector<int*> > thread_pairs;
    for(int i = 0; i < thread_num; i++){
        vector<int*> pairs;
        thread_pairs.push_back(pairs);
    }
    int *temp = new int[2];
    for(int i = 0; i < switches; i++){
        for (int j = i+1; j < switches; j++)
        {
            temp = new int[2];
            temp[0] = i;
            temp[1] = j;
            thread_pairs[allo_index].push_back(temp);
            if(count < average)
                count++;
            else{
                count = 0;
                allo_index++;
            }
        } 
    }

    string file_dir_name("");
    if(if_store){
        if(access("all_graph_infor", 0)){
            string cmd("mkdir ");
            cmd += "all_graph_infor";
            int temp = system(cmd.c_str());
        }
        file_dir_name += "sw";
        file_dir_name += to_string(switches);
        file_dir_name += "_vir";
        for(int i = 0; i < layer_num; i++)
            file_dir_name += to_string(vir_layer_degree[i]);
        file_dir_name += "_rand";
        file_dir_name += to_string(is_random*random_seed);
        if(access(("all_graph_infor/" + file_dir_name).c_str(), 0)){
            string cmd("mkdir ");
            cmd += "all_graph_infor/";
            cmd += file_dir_name;
            int temp = system(cmd.c_str());
        }
    }

    thread* th = new thread[thread_num];
    for(int i = 0; i < thread_num; i++){
        th[i] = thread(&Fc_topo_all_route::thread_route, this, thread_pairs[i], i, if_report, report_inter, if_store, store_num, file_dir_name);
    }
    for(int i = 0; i < thread_num; i++)
        th[i].join();
}

int main(){
    int switches = 1000;
    int hosts = 24;
    int ports = 64;
    int vir_layer_degree[] = {7, 13, 13, 7};
    int layer_num = 4;
    int is_random = 0;
    int random_seed = 2;
    Fc_topo_all_route fc_test(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed);
    fc_test.fc_topo_gene();
    fc_test.path_infor_gene();
    // fc_test.display_all_path();
    fc_test.build_search_dic();
    // fc_test.display_dic(2);

    struct timeval start, end;
    gettimeofday(&start, NULL);
    bool if_report = true;
    int report_inter = 30000;
    bool if_store = true;
    int store_num = 30000;
    fc_test.pthread_for_all_route(8, if_report, report_inter, if_store, store_num);
    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}
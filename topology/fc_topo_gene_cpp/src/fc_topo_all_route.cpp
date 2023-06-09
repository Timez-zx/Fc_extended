#include "fc_topo_all_route.h"


Fc_topo_all_route::~Fc_topo_all_route(){
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

    int index_basic = 0;
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
    cout << "Topology constructed" << endl;
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
    if(topo_index == NULL){
        cout << "Please generate topo!" << endl;
        exit(1);
    }
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
    if(all_path_infor == NULL){
        cout<< "Please generate index dic!" << endl;
        exit(1);
    }
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


uint Fc_topo_all_route::extract_route_path(int src, int dst, bool if_display, uint* return_graph){
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
            node_path.push_back(layer_num - loc);
            node_path.push_back(path[loc]);
            loc--;
            while(path[loc] != src){
                node_path.push_back(layer_num - loc);
                node_path.push_back(path[loc]);
                loc--;
            }
            node_path.push_back(layer_num - loc);
            node_path.push_back(path[loc]);
            reverse(node_path.begin(), node_path.end());
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
                int begin_dst = 1;
                // while(src_path[begin_src] == dst_path[begin_dst]){
                //     if(src_path[begin_src] == inter[i]){
                //         begin_src++;
                //         begin_dst++;
                //     }
                //     else
                //         break;
                // }
                // begin_src--;
                vector<int> pass_node;
                int pass_flag = 0;
                for(int k = src_path[layer_num]; k >= begin_src; k--){
                    node_path.push_back(src_path[k]);
                    pass_node.push_back(src_path[k]);
                    node_path.push_back(layer_num - k);
                }
                for(int k = begin_dst; k <= dst_path[layer_num]; k++){
                    if(find(pass_node.begin(), pass_node.end(), dst_path[k]) != pass_node.end() && dst_path[k] != inter[i]){
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
        for(int j = 0; j < route_node_path[i].size()-2; j += 2){
            vector<int> temp;
            temp.clear();
            temp.push_back(route_node_path[i][j]);
            temp.push_back(route_node_path[i][j+1]);
            temp.push_back(route_node_path[i][j+2]);
            temp.push_back(route_node_path[i][j+3]);
            graph_infor.insert(temp);
        }
    }

    graph_infor_vec.assign(graph_infor.begin(), graph_infor.end());
    uint data_count = 0;
    uint temp = src;
    return_graph[data_count] = temp;
    temp = dst;
    return_graph[data_count] += temp << 16;
    data_count++;
    for(int i = 0; i < graph_infor_vec.size(); i++){
        temp = (graph_infor_vec[i][1]-1)*switches + graph_infor_vec[i][0];
        return_graph[data_count] = temp;
        temp = (graph_infor_vec[i][3]-1)*switches + graph_infor_vec[i][2];
        return_graph[data_count] += temp << 16;
        data_count++;
    }
    return data_count;
}


void Fc_topo_all_route::thread_route(vector<int*> route_pairs, int thread_label, bool if_report, int report_inter, bool if_store, string store_file, bool store_part) {
    int count = 0;
    int store_count = 0;
    uint* temp_infor = new uint[switches*10*4];
    FILE* ofs;
    FILE* ofs_len;
    uint** store_graph_info = new uint*[report_inter];
    vector<uint> store_info_len;
    if(if_store){
        string file_path("data/all_graph_infor/" + store_file + "/" + store_file + to_string(thread_label));
        string len_path(file_path + "_num");
        ofs = fopen(file_path.c_str(), "w");
        ofs_len = fopen(len_path.c_str(), "w");
    }
    for(int i = 0; i < route_pairs.size(); i++){
        uint data_num  = extract_route_path(route_pairs[i][0], route_pairs[i][1], false, temp_infor);
        if(if_report){
            count++;
            if(count % report_inter == 0){
                cout << "The thread " << thread_label << " " <<count/double(route_pairs.size()) << endl;
            }
        }
        if(if_store){
            uint *temp_data = new uint[data_num];
            memcpy(temp_data, temp_infor, sizeof(uint)*data_num);
            store_graph_info[store_count] = temp_data;
            store_info_len.push_back(data_num);
            store_count++;
            if(store_count == report_inter) {
                fwrite(&store_info_len[0], sizeof(uint), store_count, ofs_len);
                for(int j = 0; j < report_inter; j++){
                    fwrite(store_graph_info[j], sizeof(uint), store_info_len[j], ofs);
                    delete[] store_graph_info[j];
                    store_graph_info[j] = NULL;
                }
                fflush(ofs);
                store_info_len.clear();
                store_count = 0;
                if(store_part)
                    break;
            }
        }
    }
    if(if_store){
        fwrite(&store_info_len[0], sizeof(uint), store_count, ofs_len);
        for(int j = 0; j < store_count; j++){
            fwrite(store_graph_info[j], sizeof(uint), store_info_len[j], ofs);
            delete[] store_graph_info[j];
        }
        fclose(ofs);
        fclose(ofs_len);
    }
    delete[] temp_infor;
    temp_infor = NULL;
    delete[] store_graph_info;
    store_graph_info = NULL;
}


void Fc_topo_all_route::pthread_for_all_route(int thread_num, bool if_report, int report_inter, bool if_store, bool store_part){
    if(topo_dic == NULL){
        cout << "Please generate topo dic!" << endl;
        exit(1);
    }
    int total_pairs = switches*(switches-1)/2;
    int average = ceil(total_pairs/float(thread_num));
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
            if(count < average - 1)
                count++;
            else{
                count = 0;
                allo_index++;
            }
        } 
    }

    string file_dir_name("");
    if(if_store){
        file_dir_name += gene_path_for_file("data/all_graph_infor/");
    }
    thread* th = new thread[thread_num];
    for(int i = 0; i < thread_num; i++){
        th[i] = thread(&Fc_topo_all_route::thread_route, this, thread_pairs[i], i, if_report, report_inter, if_store, file_dir_name, store_part);
    }
    for(int i = 0; i < thread_num; i++)
        th[i].join();
}


uint16_t Fc_topo_all_route::extract_all_path(int src, int dst, bool if_display, string mode, int ksp_num, uint16_t* return_path, uint16_t *path_num){
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
            node_path.push_back(layer_num - loc);
            node_path.push_back(path[loc]);
            loc--;
            while(path[loc] != src){
                node_path.push_back(layer_num - loc);
                node_path.push_back(path[loc]);
                loc--;
            }
            node_path.push_back(layer_num - loc);
            node_path.push_back(path[loc]);
            reverse(node_path.begin(), node_path.end());
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
                int begin_dst = 1;
                vector<int> pass_node;
                int pass_flag = 0;
                for(int k = src_path[layer_num]; k >= begin_src; k--){
                    node_path.push_back(src_path[k]);
                    pass_node.push_back(src_path[k]);
                    node_path.push_back(layer_num - k);
                }
                for(int k = begin_dst; k <= dst_path[layer_num]; k++){
                    if(find(pass_node.begin(), pass_node.end(), dst_path[k]) != pass_node.end() && dst_path[k] != inter[i]){
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
    vector<vector<uint16_t> > real_node_path;
    uint16_t sw1, sw2;
    uint16_t node1, node2;
    uint16_t layer1, layer2;
    uint16_t last_store = -1;
    for(int i = 0; i < route_node_path.size(); i++){
        vector<uint16_t> node_path;
        for(int j = 0; j < route_node_path[i].size()/2-1; j++){
            sw1 = route_node_path[i][2*j];
            layer1 = route_node_path[i][2*j+1];
            sw2 = route_node_path[i][2*j+2];
            layer2 = route_node_path[i][2*j+3];
            if(layer2 > layer1){
                node1 = (layer1-1)*switches+sw1;
                node2 = (layer2-1)*switches+sw2;
            }
            else{
                node1 = (layer1-1)*switches+sw1+(layer_num-layer1)*2*switches;
                node2 = (layer2-1)*switches+sw2+(layer_num-layer2)*2*switches;
            }
            if(sw1 != sw2){
                if(last_store != node1)
                    node_path.push_back(node1);
                node_path.push_back(node2);
                last_store = node2;
            }
        }
        real_node_path.push_back(node_path);
    }

    if(if_display){
        cout << real_node_path.size() << endl;
        int count = 0;
        for(int i = 0; i < real_node_path.size(); i++){
            for(int j = 0; j < real_node_path[i].size(); j++){
                cout << real_node_path[i][j] << " ";
                count++;
            }
            cout << endl;
        }
        cout<< count <<endl;
    }

    uint16_t data_count = 0;
    int path_real_len[real_node_path.size()];
    int len_index[real_node_path.size()];
    memset(path_real_len, 0, sizeof(int)*real_node_path.size());
    if(mode == "ksp" && ksp_num < real_node_path.size()){
        *path_num = ksp_num;
        for(int i = 0; i < real_node_path.size(); i++){
            len_index[i] = i;
            for(int j = 0; j < real_node_path[i].size()-1; j++){
                if(real_node_path[i][j]%switches != real_node_path[i][j+1]%switches)
                    path_real_len[i]++;
            }
        }
        sort(len_index, len_index + real_node_path.size(),
            [&](const int& a, const int& b) {return (path_real_len[a] < path_real_len[b]);});
        
        for(int i = 0; i < ksp_num; i++){
            for(int j = 0; j < real_node_path[len_index[i]].size(); j++){
                return_path[data_count+j] = real_node_path[len_index[i]][j];
            }
            data_count += real_node_path[len_index[i]].size();
        }     
    }
    else{
        *path_num = real_node_path.size();
        for(int i = 0; i < real_node_path.size(); i++){
            for(int j = 0; j < real_node_path[i].size(); j++){
                return_path[data_count+j] = real_node_path[i][j];
            }
            data_count += real_node_path[i].size();
        }
    }
    return data_count;
}


void Fc_topo_all_route::thread_all_path(vector<int*> route_pairs, int thread_label, bool if_report, int report_inter, bool if_store, string store_file, string mode, int ksp_num) {
    int count = 0;
    int store_count = 0;
    uint16_t* temp_infor = new uint16_t[switches*1000];
    FILE* ofs;
    FILE* ofs_len;
    uint16_t** store_graph_info = new uint16_t*[report_inter];
    vector<uint16_t> store_info_len;
    uint16_t path_num;
    if(if_store){
        string file_path("data/all_graph_route/" + store_file + "/" + store_file + to_string(thread_label));
        string len_path(file_path + "_num");
        ofs = fopen(file_path.c_str(), "w");
        ofs_len = fopen(len_path.c_str(), "w");
    }
    for(int i = 0; i < route_pairs.size(); i++){
        uint16_t data_num  = extract_all_path(route_pairs[i][0], route_pairs[i][1], false, mode, ksp_num, temp_infor, &path_num);
        if(if_report){
            count++;
            if(count % report_inter == 0){
                cout << "The thread " << thread_label << " " <<count/double(route_pairs.size()) << endl;
            }
        }
        if(if_store){
            uint16_t *temp_data = new uint16_t[data_num];
            memcpy(temp_data, temp_infor, sizeof(uint16_t)*data_num);
            store_graph_info[store_count] = temp_data;
            store_info_len.push_back(path_num);
            store_info_len.push_back(data_num);
            store_count++;
            if(store_count == report_inter) {
                fwrite(&store_info_len[0], sizeof(uint16_t), store_count*2, ofs_len);
                for(int j = 0; j < report_inter; j++){
                    fwrite(store_graph_info[j], sizeof(uint16_t), store_info_len[2*j+1], ofs);
                    delete[] store_graph_info[j];
                    store_graph_info[j] = NULL;
                }
                fflush(ofs);
                store_info_len.clear();
                store_count = 0;
            }
        }
    }
    if(if_store){
        fwrite(&store_info_len[0], sizeof(uint16_t), store_count*2, ofs_len);
        for(int j = 0; j < store_count; j++){
            fwrite(store_graph_info[j], sizeof(uint16_t), store_info_len[2*j+1], ofs);
            delete[] store_graph_info[j];
        }
        fclose(ofs);
        fclose(ofs_len);
    }
    delete[] temp_infor;
    temp_infor = NULL;
    delete[] store_graph_info;
    store_graph_info = NULL;
}


void Fc_topo_all_route::pthread_for_all_path(int thread_num, bool if_report, int report_inter, bool if_store, string mode, int ksp_num){
    if(topo_dic == NULL){
        cout << "Please generate topo dic!" << endl;
        exit(1);
    }
    int total_pairs = switches*(switches-1)/2;
    int average = ceil(total_pairs/float(thread_num));
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
            if(count < average - 1)
                count++;
            else{
                count = 0;
                allo_index++;
            }
        } 
    }

    string file_dir_name("");
    if(if_store){
        file_dir_name += gene_path_for_file("data/all_graph_route/");
    }
    thread* th = new thread[thread_num];
    for(int i = 0; i < thread_num; i++){
        th[i] = thread(&Fc_topo_all_route::thread_all_path, this, thread_pairs[i], i, if_report, report_inter, if_store, file_dir_name, mode, ksp_num);
    }
    for(int i = 0; i < thread_num; i++)
        th[i].join();

    string file_path("data/all_graph_route/" + file_dir_name + "/" + file_dir_name);
    string len_path(file_path + "_num"); 
    int state;
    FILE* ofs = fopen(file_path.c_str(), "w");
    FILE* ofs_len = fopen(len_path.c_str(), "w");
    for(int i = 0; i < thread_num; i++){
        string in_file_path("data/all_graph_route/" + file_dir_name + "/" + file_dir_name + to_string(i));
        string in_len_path(in_file_path + "_num");
        string cmd1("cat ");
        string cmd2("cat ");
        cmd1 += in_file_path;
        cmd1 += " >> ";
        cmd1 += file_path;
        cmd2 += in_len_path;
        cmd2 += " >> ";
        cmd2 += len_path;
        state = system(cmd1.c_str());
        state = system(cmd2.c_str());
        cmd1 = "rm " + in_file_path;
        cmd2 = "rm " + in_len_path;
        state = system(cmd1.c_str());
        state = system(cmd2.c_str());
    }
    fclose(ofs);
    fclose(ofs_len);
}

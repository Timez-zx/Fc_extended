#include "fc_edge_disjoin_route.h"


void Fc_edge_disjoin_route::find_all_route(int thread_num, int batch_num, bool if_search_map){
    if(access("all_graph_route", 0)){
        string cmd("mkdir ");
        cmd += "all_graph_route";
        int temp = system(cmd.c_str());
    }
    string file_dir_name("");
    file_dir_name += "sw";
    file_dir_name += to_string(switches);
    file_dir_name += "_vir";
    for(int i = 0; i < layer_num; i++)
        file_dir_name += to_string(vir_layer_degree[i]);
    file_dir_name += "_rand";
    file_dir_name += to_string(is_random*random_seed);
    if(access(("all_graph_route/" + file_dir_name).c_str(), 0)){
        string cmd("mkdir ");
        cmd += "all_graph_route/";
        cmd += file_dir_name;
        int temp = system(cmd.c_str());
    }
    thread* th = new thread[thread_num];
    for(int i = 0; i < thread_num; i++){
        th[i] = thread(&Fc_edge_disjoin_route::find_edge_disjoin_route_fast, this, thread_num, i, batch_num, file_dir_name, false);
    }
    for(int i = 0; i < thread_num; i++)
        th[i].join();

    string file_path("all_graph_route/" + file_dir_name + "/" + file_dir_name);
    string len_path(file_path + "_num"); 
    int state;
    FILE* ofs = fopen(file_path.c_str(), "w");
    FILE* ofs_len = fopen(len_path.c_str(), "w");
    for(int i = 0; i < thread_num; i++){
        string in_file_path("all_graph_route/" + file_dir_name + "/" + file_dir_name + to_string(i));
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


void Fc_edge_disjoin_route::find_edge_disjoin_route_fast(int thread_num, int thread_label, int batch_num, string read_file, bool store_part){
    int total_pairs = switches*(switches-1)/2;
    int average = ceil(total_pairs/float(thread_num));
    int pairs_num = average;
    if(thread_label == thread_num - 1){
        pairs_num = total_pairs - (thread_num-1)*average;
    }
    if(store_part)
        pairs_num = batch_num;
    int pairs = pairs_num;
    string file_path("all_graph_infor/" + read_file + "/" + read_file + to_string(thread_label));
    string len_path(file_path + "_num");
    FILE* ifs = fopen(file_path.c_str(), "r");
    FILE* ifs_len = fopen(len_path.c_str(), "r");

    string out_file_path("all_graph_route/" + read_file + "/" + read_file + to_string(thread_label));
    string out_len_path(out_file_path + "_num");
    FILE* ofs = fopen(out_file_path.c_str(), "w");
    FILE* ofs_len = fopen(out_len_path.c_str(), "w");

    int read_num;
    uint edge_num[pairs_num];
    int temp = fread(edge_num, 4, pairs_num, ifs_len);
    int max_len = 0;
    for(int i = 0; i < pairs_num; i++){
        if(max_len <= edge_num[i])
            max_len = edge_num[i];
    }
    uint16_t *edge_infor[batch_num];
    for(int i = 0; i < batch_num; i++)
        edge_infor[i] = new uint16_t[max_len*2];

    int total_switches = (2*layer_num-1)*switches;
    int reverse_index_table[total_switches+2];
    vector<int> index_table;
    memset(reverse_index_table, 0xff, sizeof(int)*(total_switches+2));
    int index_count = 0;
    int count = 0;
    int src, dst;
    int vir_src = total_switches;
    int vir_dst = total_switches + 1;
    reverse_index_table[vir_src] = index_count;
    index_table.push_back(vir_src);
    index_count++;
    reverse_index_table[vir_dst] = index_count;
    index_table.push_back(vir_dst);
    index_count++;

    int node1, node2;
    int sw1, sw2;
    int layer1, layer2;
    int average_num = 0;
    int min_path_num = 1000;
    float average_len = 0;

    Edge edges[MAX_NUM];
    int visited[MAX_NUM];
    int heads[MAX_NUM];
    int edges_count = 0;
    memset(visited, 0, sizeof(int)*MAX_NUM);
    memset(heads, 0xff, sizeof(int)*MAX_NUM);
    memset(edges, 0xff, sizeof(Edge)*MAX_NUM);

    int status, max_flow;
    int path_len, real_len, last_store = -1;
    uint16_t* path_route = new uint16_t[2*layer_num+1];
    uint16_t* real_path_route = new uint16_t[2*layer_num+1];

    uint16_t *pair_len = new uint16_t[batch_num*2];
    uint16_t **pair_route = new uint16_t*[batch_num];
    for(int i = 0; i < batch_num; i++)
        pair_route[i] = new uint16_t[(2*layer_num+1)*200];
    
    uint16_t single_pair_len;
    uint16_t *single_pair_route = new uint16_t[(2*layer_num+1)*200];
     

    while(pairs_num > 0){
        if(pairs_num >= batch_num)
            read_num = batch_num;
        else
            read_num = pairs_num;
        for(int i = 0; i < read_num; i++){
            temp = fread(edge_infor[i], 2, edge_num[count]*2, ifs);
            count++;
        }
        count -= read_num;

        for(int i = 0; i < read_num; i++){
            operations_research::SimpleMinCostFlow min_cost_flow;
            min_cost_flow.AddArcWithCapacityAndUnitCost(reverse_index_table[vir_dst], reverse_index_table[vir_src], INT32_MAX, -10000);
            src = edge_infor[i][0];
            dst = edge_infor[i][1];
            for(int j = 0; j < layer_num; j++){
                reverse_index_table[j*switches+src] = index_count;
                min_cost_flow.AddArcWithCapacityAndUnitCost(reverse_index_table[vir_src], index_count, INT32_MAX, 0);
                index_table.push_back(j*switches+src);
                index_count++;
                if(j == layer_num - 1){
                    reverse_index_table[(layer_num-1)*switches+dst] = index_count;
                    min_cost_flow.AddArcWithCapacityAndUnitCost(index_count, reverse_index_table[vir_dst], INT32_MAX, 0);
                    index_table.push_back((layer_num-1)*switches+dst);
                }
                else{
                    reverse_index_table[(layer_num+j)*switches+dst] = index_count;
                    min_cost_flow.AddArcWithCapacityAndUnitCost(index_count, reverse_index_table[vir_dst], INT32_MAX, 0);
                    index_table.push_back((layer_num+j)*switches+dst);
                }
                index_count++;
            }

            for(int j = 0; j < layer_num-1; j++){
                min_cost_flow.AddArcWithCapacityAndUnitCost(reverse_index_table[j*switches+src], reverse_index_table[(j+1)*switches+src], INT32_MAX, 0);
                node1 = j*switches+dst;
                node2 = (j+1)*switches+dst;
                if(reverse_index_table[node1] == -1){
                    reverse_index_table[node1] = index_count;
                    index_table.push_back(node1);
                    index_count++;
                }
                if(reverse_index_table[node2] == -1){
                    reverse_index_table[node2] = index_count;
                    index_table.push_back(node2);
                    index_count++;
                }
                min_cost_flow.AddArcWithCapacityAndUnitCost(reverse_index_table[node1],reverse_index_table[node2],INT32_MAX, 0);
            }

            for(int j = 0; j < layer_num-1; j++){
                node1 = (layer_num+j-1)*switches+src;
                node2 = (layer_num+j)*switches+src;
                if(reverse_index_table[node1] == -1){
                    reverse_index_table[node1] = index_count;
                    index_table.push_back(node1);
                    index_count++;
                }
                if(reverse_index_table[node2] == -1){
                    reverse_index_table[node2] = index_count;
                    index_table.push_back(node2);
                    index_count++;
                }
                min_cost_flow.AddArcWithCapacityAndUnitCost(reverse_index_table[node1],reverse_index_table[node2], INT32_MAX, 0);
                min_cost_flow.AddArcWithCapacityAndUnitCost(reverse_index_table[(layer_num+j-1)*switches+dst], reverse_index_table[(layer_num+j)*switches+dst], INT32_MAX, 0);
            }

            for(int j = 2; j < edge_num[count]*2; j += 2){
                node1 = edge_infor[i][j];
                node2 = edge_infor[i][j+1];
                sw1 = node1%switches;
                layer1 = node1/switches;
                sw2 = node2%switches;
                layer2 = node2/switches;
                node1 = layer1*switches+sw1;
                node2 = layer2*switches+sw2;
                if(node2 < node1){
                    node1 += (layer_num-layer1-1)*2*switches;
                    node2 += (layer_num-layer2-1)*2*switches;
                }
                if(reverse_index_table[node1] == -1){
                    reverse_index_table[node1] = index_count;
                    index_table.push_back(node1);
                    index_count++;
                }
                if(reverse_index_table[node2] == -1){
                    reverse_index_table[node2] = index_count;
                    index_table.push_back(node2);
                    index_count++;
                }
                if(sw1 != sw2){
                    // edge_disjoint may not be the best
                    min_cost_flow.AddArcWithCapacityAndUnitCost(reverse_index_table[node1],reverse_index_table[node2], 1, 1);
                }
                else{
                    min_cost_flow.AddArcWithCapacityAndUnitCost(reverse_index_table[node1],reverse_index_table[node2], INT32_MAX, 0);
                }
            }
            status = min_cost_flow.Solve();
            count++;
            max_flow = 0;
            int real_flow_count = 0;
            if(status == min_cost_flow.OPTIMAL){
                max_flow = min_cost_flow.Flow(0);
                for(int j = 1; j < min_cost_flow.NumArcs(); j++){
                    if(min_cost_flow.Flow(j) > 0){
                        edges[edges_count].weight = min_cost_flow.Flow(j);
                        edges[edges_count].to = min_cost_flow.Head(j);
                        edges[edges_count].next = heads[min_cost_flow.Tail(j)];
                        heads[min_cost_flow.Tail(j)] = edges_count++;
                    }
                }
                single_pair_len = 0;
                for(int j = 0; j < max_flow; j++){
                    real_len = 0;
                    path_len = dfs(heads, visited, edges, 0, 1, path_route);
                    for(int k = path_len - 1; k >= 0; k--){
                        if(index_table[path_route[k]]%switches != index_table[path_route[k-1]]%switches){
                            if(last_store != index_table[path_route[k]]){
                                real_path_route[real_len++] = index_table[path_route[k]];
                            }
                            real_path_route[real_len++] = index_table[path_route[k-1]];
                            last_store = index_table[path_route[k-1]];
                            if(index_table[path_route[k-1]]%switches == dst)
                                break;
                        }
                    }
                    for(int k = 0; k < real_len-1; k++){
                        node1 = real_path_route[k];
                        node2 = real_path_route[k+1];
                        sw1 = node1%switches;
                        sw2 = node2%switches;
                        if(sw1 != sw2)
                            real_flow_count++;
                    }
                    memcpy(single_pair_route+single_pair_len, real_path_route, sizeof(uint16_t)*real_len);
                    single_pair_len += real_len;
                    memset(visited, 0, sizeof(int)*MAX_NUM);
                }
                pair_len[2*i] = max_flow;
                pair_len[2*i+1] = single_pair_len;
                memcpy(pair_route[i], single_pair_route, sizeof(uint16_t)*single_pair_len);
            }
            else
                cout << "error" << endl;
            average_len += real_flow_count/float(max_flow);
            average_num += max_flow;
            if(max_flow < min_path_num){
                min_path_num = max_flow;
            }
            memset(reverse_index_table, 0xff, sizeof(int)*total_switches);
            index_count = 2;
            index_table.clear();
            index_table.push_back(vir_src);
            index_table.push_back(vir_dst);
            edges_count = 0;
            memset(heads, 0xff, sizeof(int)*MAX_NUM);
            memset(visited, 0x00, sizeof(int)*MAX_NUM);
            memset(edges, 0xff, sizeof(Edge)*MAX_NUM);
        }
        fwrite(pair_len, sizeof(uint16_t), read_num*2, ofs_len);
        for(int i = 0; i < read_num; i++){
            fwrite(pair_route[i], sizeof(uint16_t), pair_len[2*i+1], ofs);
        }
        pairs_num -= read_num;
        cout << "Thread "<< thread_label << " left: "<< pairs_num/float(pairs) << endl;
    }
    if(store_part){
        cout << "The average num for thread "<< thread_label << ": "<< average_num/float(batch_num) << endl;
        cout << "The average length for thread "<< thread_label << ": "<< average_len/float(batch_num) << endl;
    }
    else{
        cout << "The average num for thread "<< thread_label << ": " << average_num/float(pairs) << endl;
        cout << "The average length for thread "<< thread_label << ": " << average_len/float(pairs) << endl;        
    }
    for(int i = 0; i < batch_num; i++){
        delete[] edge_infor[i];
        edge_infor[i] = NULL;
        delete[] pair_route[i];
        pair_route[i] = NULL;
    }
    delete[] path_route;
    delete[] real_path_route;
    delete[] pair_len;
    delete[] pair_route;
    delete[] single_pair_route;
    fclose(ifs);
    fclose(ofs);
    fclose(ofs_len);
    fclose(ifs_len);
}


int Fc_edge_disjoin_route::dfs(int* head, int* visited, Edge* edge, int src, int dst, uint16_t* store_path){
    visited[src] = 1;
    for(int i = head[src]; i != -1; i = edge[i].next){
        if(edge[i].weight == 0)
            continue;
        if(!visited[edge[i].to]){
            if(edge[i].to == dst){
                edge[i].weight--;
                store_path[0] = dst;
                return 1;
            }
            else{
                int temp_len = dfs(head, visited, edge, edge[i].to, dst, store_path);
                store_path[temp_len] = edge[i].to;
                edge[i].weight--;
                return ++temp_len;
            }
        }
    }
    return 0;
}


int Fc_edge_disjoin_route::verify_route(int src, int dst){
    operations_research::SimpleMinCostFlow min_cost_flow;
    int vir_src = switches;
    int vir_dst = switches+1;
    min_cost_flow.AddArcWithCapacityAndUnitCost(vir_dst, vir_src, 10000, -10000);

    int degree;
    int node1, node2;
    int basic = 0;
    for(int i = 0; i < layer_num - 1; i++){
        degree = bipart_degree[i];
        for(int j = 0; j < switches; j++){

            for (int k = 0; k < degree; k++)
            {
                if(i == 0){
                    node1 = j + 10000*(layer_num-i);
                    node2 = topo_index[basic+j*degree+k] + 10000*(layer_num-i-1) + 1e4*layer_num;
                    if(j == topo_index[basic+j*degree+k])
                        min_cost_flow.AddArcWithCapacityAndUnitCost(node1, node2, INT32_MAX, 0);
                    else
                        min_cost_flow.AddArcWithCapacityAndUnitCost(node1, node2, 1, 1);
                    node1 = topo_index[basic+j*degree+k] + 10000*(layer_num-i-1);
                    node2 = j + 10000*(layer_num-i);
                    if(j == topo_index[basic+j*degree+k])
                        min_cost_flow.AddArcWithCapacityAndUnitCost(node1, node2, INT32_MAX, 0);
                    else
                        min_cost_flow.AddArcWithCapacityAndUnitCost(node1, node2, 1, 1); 
                }
                else{
                    node1 = j + 10000*(layer_num-i) + 1e4*layer_num;
                    node2 = topo_index[basic+j*degree+k] + 10000*(layer_num-i-1) + 1e4*layer_num;
                    if(j == topo_index[basic+j*degree+k])
                        min_cost_flow.AddArcWithCapacityAndUnitCost(node1, node2, INT32_MAX, 0);
                    else
                        min_cost_flow.AddArcWithCapacityAndUnitCost(node1, node2, 1, 1);
                    node1 = topo_index[basic+j*degree+k] + 10000*(layer_num-i-1);
                    node2 = j + 10000*(layer_num-i);
                    if(j == topo_index[basic+j*degree+k])
                        min_cost_flow.AddArcWithCapacityAndUnitCost(node1, node2, INT32_MAX, 0);
                    else
                        min_cost_flow.AddArcWithCapacityAndUnitCost(node1, node2, 1, 1);   
                }
            }
            
        }
        basic += switches*degree;
    }
    for(int i = 1; i <= layer_num; i++){
        min_cost_flow.AddArcWithCapacityAndUnitCost(vir_src, i*10000+src, INT32_MAX, 0);
        if(i == layer_num)
            min_cost_flow.AddArcWithCapacityAndUnitCost(i*10000+dst, vir_dst, INT32_MAX, 0);
            
        else
            min_cost_flow.AddArcWithCapacityAndUnitCost(1e4*layer_num+i*10000+dst, vir_dst, INT32_MAX, 0);
    }
    min_cost_flow.Solve();
    int max_flow = 0;
    if(min_cost_flow.OPTIMAL)
        max_flow = min_cost_flow.Flow(0);
    else    
        cout << "error\n";
    return max_flow;
}
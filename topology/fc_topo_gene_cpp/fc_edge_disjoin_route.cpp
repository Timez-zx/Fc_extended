#include "fc_edge_disjoin_route.h"

void Fc_edge_disjoin_route::find_edge_disjoin_route(int thread_num, int thread_label, int batch_num, string read_file, bool store_part){
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

    int count = 0;
    int src, dst;
    int vir_src = total_switches;
    int vir_dst = total_switches + 1;
    int node1, node2;
    int sw1, sw2;
    int layer1, layer2;
    int average_num = 0;
    int min_path_num = 1000;
    float average_len = 0;
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
            min_cost_flow.AddArcWithCapacityAndUnitCost(vir_dst, vir_src, INT32_MAX, -10000);
            src = edge_infor[i][0];
            dst = edge_infor[i][1];
            for(int j = 0; j < layer_num; j++){
                min_cost_flow.AddArcWithCapacityAndUnitCost(vir_src, j*switches+src, INT32_MAX, 0);
                if(j == layer_num - 1)
                    min_cost_flow.AddArcWithCapacityAndUnitCost((layer_num-1)*switches+dst, vir_dst, INT32_MAX, 0);
                else
                    min_cost_flow.AddArcWithCapacityAndUnitCost((layer_num+j)*switches+dst, vir_dst, INT32_MAX, 0);
            }

            for(int j = 0; j < layer_num-1; j++){
                min_cost_flow.AddArcWithCapacityAndUnitCost(j*switches+src, (j+1)*switches+src,INT32_MAX, 0);
                min_cost_flow.AddArcWithCapacityAndUnitCost(j*switches+dst, (j+1)*switches+dst,INT32_MAX, 0);
            }

            for(int j = 0; j < layer_num-1; j++){
                min_cost_flow.AddArcWithCapacityAndUnitCost((layer_num+j-1)*switches+src, (layer_num+j)*switches+src, INT32_MAX, 0);
                min_cost_flow.AddArcWithCapacityAndUnitCost((layer_num+j-1)*switches+dst, (layer_num+j)*switches+dst, INT32_MAX, 0);
            }

            for(int j = 2; j < edge_num[count]*2; j += 2){
                node1 = edge_infor[i][j];
                node2 = edge_infor[i][j+1];
                sw1 = node1%10000;
                layer1 = node1/10000;
                sw2 = node2%10000;
                layer2 = node2/10000;
                node1 = (layer1-1)*switches+sw1;
                node2 = (layer2-1)*switches+sw2;
                if(node2 < node1){
                    node1 += (layer_num-layer1)*2*switches;
                    node2 += (layer_num-layer2)*2*switches;
                }
                if(sw1 != sw2){
                    min_cost_flow.AddArcWithCapacityAndUnitCost(node1, node2, 1, 1);
                }
                else{
                    min_cost_flow.AddArcWithCapacityAndUnitCost(node1, node2, INT32_MAX, 0);
                }
            }
            int status = min_cost_flow.Solve();
            // int verify = verify_route(src, dst);
            count++;
            int max_flow = 0;
            if(status == min_cost_flow.OPTIMAL)
                max_flow = min_cost_flow.Flow(0);
            else
                cout << "error" << endl;
            // if(verify != max_flow){
            //     cout << src << "->" << dst << ":verify:" << verify << " " << max_flow << endl;
            // }
            average_len += (max_flow*10000+min_cost_flow.OptimalCost())/float(max_flow);
            average_num += max_flow;
            if(max_flow< min_path_num){
                min_path_num = max_flow;
            }

        }
        pairs_num -= read_num;
        cout << pairs_num << endl;
    }
    if(store_part){
        cout << average_num/float(batch_num) << endl;
        cout << average_len/float(batch_num) << endl;
    }
    else{
        cout << average_num/float(pairs) << endl;
        cout << average_len/float(pairs) << endl;        
    }
    for(int i = 0; i < batch_num; i++){
        delete[] edge_infor[i];
        edge_infor[i] = NULL;
    }
    fclose(ifs);
    fclose(ifs_len);
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
                sw1 = node1%10000;
                layer1 = node1/10000;
                sw2 = node2%10000;
                layer2 = node2/10000;
                node1 = (layer1-1)*switches+sw1;
                node2 = (layer2-1)*switches+sw2;
                if(node2 < node1){
                    node1 += (layer_num-layer1)*2*switches;
                    node2 += (layer_num-layer2)*2*switches;
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
                    min_cost_flow.AddArcWithCapacityAndUnitCost(reverse_index_table[node1],reverse_index_table[node2], 1, 1);
                }
                else{
                    min_cost_flow.AddArcWithCapacityAndUnitCost(reverse_index_table[node1],reverse_index_table[node2], INT32_MAX, 0);
                }
            }
            int status = min_cost_flow.Solve();
            count++;
            int max_flow = 0;
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
                for(int j = 0; j < max_flow; j++){
                    dfs(heads, visited, edges, edges_count, 0, 1);
                    memset(visited, 0, sizeof(int)*MAX_NUM);
                    cout << 0 << endl;
                }
            }
            else
                cout << "error" << endl;
            average_len += (max_flow*10000+min_cost_flow.OptimalCost())/float(max_flow);
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
        pairs_num -= read_num;
        cout << pairs_num << endl;
    }
    if(store_part){
        cout << average_num/float(batch_num) << endl;
        cout << average_len/float(batch_num) << endl;
    }
    else{
        cout << average_num/float(pairs) << endl;
        cout << average_len/float(pairs) << endl;        
    }
    for(int i = 0; i < batch_num; i++){
        delete[] edge_infor[i];
        edge_infor[i] = NULL;
    }
    fclose(ifs);
    fclose(ifs_len);
}


void Fc_edge_disjoin_route::dfs(int* head, int* visited, Edge* edge, int edge_count, int src, int dst){
    // for(int j = 0; j <= edge_count; j++){
    //     for(int k = head[j]; k != -1; k=edge[k].next){
    //         cout << j << "->" << edge[k].to << ":" << edge[k].weight << endl;
    //     }
    // }
    visited[src] = 1;
    for(int i = head[src]; i != -1; i = edge[i].next){
        if(edge[i].weight == 0)
            continue;
        if(!visited[edge[i].to]){
            if(edge[i].to == dst){
                edge[i].weight--;
                cout << dst << " ";
                return;
            }
            else{
                dfs(head, visited, edge, edge_count, edge[i].to, dst);
                cout << edge[i].to << " ";
                edge[i].weight--;
                return;
            }
        }
    }



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
#include "fc_edge_disjoin_route.h"

void Fc_edge_disjoin_route::find_edge_disjoin_route(int thread_num, int thread_label, int batch_num, string read_file){
    int total_pairs = switches*(switches-1)/2;
    int average = 1 + total_pairs/thread_num;
    int pairs_num = average;
    if(thread_label == thread_num - 1){
        pairs_num = total_pairs - (thread_num-1)*average;
    }

    string file_path("all_graph_infor/" + read_file + "/" + read_file + to_string(thread_label));
    string len_path(file_path + "_num");
    FILE* ifs = fopen(file_path.c_str(), "r");
    FILE* ifs_len = fopen(len_path.c_str(), "r");

    int read_num;
    uint edge_num[pairs_num];
    fread(edge_num, 4, pairs_num, ifs_len);
    int max_len = 0;
    for(int i = 0; i < pairs_num; i++)
        max_len = (max_len >= edge_num[i]) ? max_len:edge_num[i];

    uint16_t *edge_infor[batch_num];
    for(int i = 0; i < batch_num; i++)
        edge_infor[i] = new uint16_t[max_len*2];

    int count = 0;
    int src, dst;
    int vir_src = switches;
    int vir_dst = -1*switches;
    int node1, node2;
    int sw1, sw2;
    while(pairs_num > 0){
        if(pairs_num > batch_num)
            read_num = batch_num;
        else
            read_num = pairs_num;

        for(int i = 0; i < read_num; i++){
            fread(edge_infor[i], 2, edge_num[count]*2, ifs);
            count++;
        }
        count -= read_num;

        for(int i = 0; i < read_num; i++){
            operations_research::SimpleMinCostFlow min_cost_flow;
            min_cost_flow.AddArcWithCapacityAndUnitCost(vir_src, vir_dst, INT32_MAX, -0xfffff);
            src = edge_infor[i][0];
            dst = edge_infor[i][1];
            for(int j = 1; j <= layer_num; j++){
                min_cost_flow.AddArcWithCapacityAndUnitCost(vir_src, j*10000+src, INT32_MAX, 0);
                if(j == layer_num)
                    min_cost_flow.AddArcWithCapacityAndUnitCost(j*10000+dst, vir_dst, INT32_MAX, 0);
                else
                    min_cost_flow.AddArcWithCapacityAndUnitCost((j*10000+dst)*-1, vir_dst, INT32_MAX, 0);
            }
            for(int j = 2; j < edge_num[count]; j += 2){
                node1 = edge_infor[i][j];
                node2 = edge_infor[i][j+1];
                sw1 = node1%10000;
                sw2 = node2%10000;
                if(node2 < node1){
                    node2 *= -1;
                    if(node1 < layer_num*10000)
                        node1 *= -1;
                }
                if(sw1 != sw2)
                    min_cost_flow.AddArcWithCapacityAndUnitCost(node1, node2, 1, 1);
                else
                    min_cost_flow.AddArcWithCapacityAndUnitCost(node1, node2, INT32_MAX, 0);
            }
            min_cost_flow.Solve();
            count++;
        }
        pairs_num -= read_num;
        cout << pairs_num << endl;
    }

    for(int i = 0; i < batch_num; i++){
        delete[] edge_infor[i];
        edge_infor[i] = NULL;
    }
    fclose(ifs);
    fclose(ifs_len);

}
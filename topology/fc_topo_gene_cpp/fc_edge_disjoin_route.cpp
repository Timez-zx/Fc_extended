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

    int count = 0;
    int src, dst;
    int vir_src = switches;
    int vir_dst = switches + 1;
    int node1, node2;
    int sw1, sw2;
    int average_num = 0;
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
            min_cost_flow.AddArcWithCapacityAndUnitCost(vir_dst, vir_src, 10000, -10000);
            src = edge_infor[i][0];
            dst = edge_infor[i][1];
            for(int j = 1; j <= layer_num; j++){
                min_cost_flow.AddArcWithCapacityAndUnitCost(vir_src, j*10000+src, INT32_MAX, 0);
                if(j == layer_num)
                    min_cost_flow.AddArcWithCapacityAndUnitCost(10000*j+dst, vir_dst, INT32_MAX, 0);
                else
                    min_cost_flow.AddArcWithCapacityAndUnitCost(1e5+10000*j+dst, vir_dst, INT32_MAX, 0);
            }

            for(int j = 1; j < layer_num; j++){
                min_cost_flow.AddArcWithCapacityAndUnitCost(j*10000+src, (j+1)*10000+src,INT32_MAX, 0);
                min_cost_flow.AddArcWithCapacityAndUnitCost(j*10000+dst, (j+1)*10000+dst,INT32_MAX, 0);
            }

            for(int j = layer_num; j > 1; j--){
                if(j == layer_num){
                    min_cost_flow.AddArcWithCapacityAndUnitCost(j*10000+src, 1e5 + (j-1)*10000+src, INT32_MAX, 0);
                    min_cost_flow.AddArcWithCapacityAndUnitCost(j*10000+dst, 1e5 + (j-1)*10000+dst, INT32_MAX, 0);
                }
                else{
                    min_cost_flow.AddArcWithCapacityAndUnitCost(1e5 + j*10000+src, 1e5 + (j-1)*10000+src,INT32_MAX, 0);
                    min_cost_flow.AddArcWithCapacityAndUnitCost(1e5 + j*10000+dst, 1e5 + (j-1)*10000+dst,INT32_MAX, 0); 
                }
            }

            for(int j = 2; j < edge_num[count]*2; j += 2){
                node1 = edge_infor[i][j];
                node2 = edge_infor[i][j+1];
                sw1 = node1%10000;
                sw2 = node2%10000;
                if(node2 < node1){
                    node2 += 1e5;
                    if(node1 < layer_num*10000)
                        node1 += 1e5;
                }
                if(sw1 != sw2){
                    min_cost_flow.AddArcWithCapacityAndUnitCost(node1, node2, 1, 1);
                }
                else
                    min_cost_flow.AddArcWithCapacityAndUnitCost(node1, node2, INT32_MAX, 0);
            }
            int status = min_cost_flow.Solve();
            count++;
            int min_cost = min_cost_flow.OptimalCost()*(-1)/10000+1;
            average_num += min_cost;

        }
        pairs_num -= read_num;
        cout << pairs_num << endl;
        break;
    }
    cout << average_num/float(batch_num) << endl;
    for(int i = 0; i < batch_num; i++){
        delete[] edge_infor[i];
        edge_infor[i] = NULL;
    }
    fclose(ifs);
    fclose(ifs_len);

}
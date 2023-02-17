#include "fc_tagger_kspz.h"


void Fc_tagger_kspz::save_graph_infor(){
    if(topo_index == NULL){
        cout << "Please generate graph infor first by fc_topo_gene_1v1!" << endl;
        exit(1);
    }
    string file_dir_name = gene_path_for_file("data/topo_infor/");
    ofstream ofs("data/topo_infor/" + file_dir_name + "/" + file_dir_name+".txt");
    int degree;
    int basic_index = 0;
    int src, dst;
    ofs << "LinkID,SourceID,DestinationID,PeerID,Cost,Bandwidth,Delay,SRLGNum,SRLGs" << endl;
    int count = 0;
    for(int i = 0; i < layer_num-1; i++){
        degree = bipart_degree[i];
        for(int j = 0; j < switches; j++){
            src = j;
            for(int k = 1; k < degree; k++){
                dst = topo_index[basic_index+j*degree+k];
                link_to_layer_map[src*switches+dst] = (layer_num-i-1)*layer_num+layer_num-i-2;
                link_to_layer_map[dst*switches+src] = (layer_num-i-2)*layer_num+layer_num-i-1;
                ofs << count << "," <<src << "," << dst << "," << 0 << "," << 1 << ",0,0,0,0"<< endl;
                count++;
                ofs << count << "," <<dst << "," << src << "," << 0 << "," << 1 << ",0,0,0,0"<< endl;
                count++;
            }
        }
        basic_index += degree*switches;
    }
    ofs.close();
    topo_path = "data/topo_infor/" + file_dir_name + "/" + file_dir_name+".txt";
}


string Fc_tagger_kspz::gene_path_for_file_ksp(string path, int ksp_num, int vc_num){
    if(access(path.c_str(), 0)){
        string cmd("mkdir ");
        cmd += path;
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
    file_dir_name += "_";
    file_dir_name += to_string(ksp_num);
    file_dir_name += "_";
    file_dir_name += to_string(vc_num);
    if(access((path + file_dir_name).c_str(), 0)){
        string cmd("mkdir ");
        cmd += path;
        cmd += file_dir_name;
        int temp = system(cmd.c_str());
    }
    return file_dir_name;
}


uint16_t Fc_tagger_kspz::search_up_down_ksp(int src, int dst, int path_num, int vc_num, uint16_t *path_infor, int thread_label){
    KShortestPath ksp(graph_pr[thread_label]);
	int i = 0;
    int path[1000];
    int layer_pass[1000];
    int path_len;
    int src_inter, dst_inter, src_layer, dst_layer;
    int past_layer = 0;
    int vc_used;
    uint16_t real_path[1000];
    uint16_t data_num = 0;
    int last_pass;
    double cost = ksp.Init(src, dst);
    double min_cost = cost;

	while(cost < 10000 & i < path_num){
        if(cost == min_cost)
            thread_ecmp_num[thread_label]++;
        vector<Link*> path_temp = ksp.GetPath();
        path_len = cost+1;
        for(int i = 0; i < path_temp.size(); i++){
            path[i] = path_temp[i]->source_id;
        }
        path[path_temp.size()] = dst;
        vc_used = 1;
        past_layer = 0;
        last_pass = -1;
        vector<int> extract_layer_pass;
        for(int i = 0; i < path_len-1; i++){
            src_inter = path[i];
            dst_inter = path[i+1];
            src_layer = link_to_layer_map[src_inter*switches+dst_inter]/layer_num;
            dst_layer = link_to_layer_map[src_inter*switches+dst_inter]%layer_num;
            layer_pass[2*i] = src_layer;
            layer_pass[2*i+1] = dst_layer;
        }
        for(int i = 0; i < 2*(path_len-1); i++){
            if(layer_pass[i] != last_pass)
                extract_layer_pass.push_back(layer_pass[i]);
            last_pass = layer_pass[i];
        }
        for(int i = 0; i < extract_layer_pass.size()-1; i++){
            src_layer = extract_layer_pass[i];
            if(src_layer < past_layer && src_layer < extract_layer_pass[i+1]){
                vc_used++;
            }
            past_layer = src_layer;
        }  
        // for(int i = 0; i < 2*(path_len-1); i++){
        //     cout << layer_pass[i] << " ";
        // }
        // cout << endl;
        cost = ksp.FindNextPath();
        if(vc_used > vc_num){
            continue;
        }
        else{
            i++;
            // for(int i = 0; i < path_len; i++){
            //     cout << path[i] << " ";
            // }
            // cout << endl << endl;
            for(int i = 0; i < path_len-1; i++){
                if(layer_pass[2*i] < layer_pass[2*i+1]){
                    real_path[2*i] = layer_pass[2*i]*switches + path[i];
                    real_path[2*i+1] = layer_pass[2*i+1]*switches + path[i+1];
                }
                else{
                    real_path[2*i] = layer_pass[2*i]*switches + path[i] + (layer_num-layer_pass[2*i]-1)*2*switches;
                    real_path[2*i+1] = layer_pass[2*i+1]*switches + path[i+1] + (layer_num-layer_pass[2*i+1]-1)*2*switches;
                }
                path_infor[data_num + 2*i] = real_path[2*i];
                path_infor[data_num + 2*i + 1] = real_path[2*i+1];

            }
            data_num += 2*(path_len-1);
        }
	}
    return data_num;
}


void Fc_tagger_kspz::thread_up_down_ksp(vector<int*> route_pairs, int thread_label, int path_num, int vc_num, bool if_report, int report_inter, bool if_store, string store_file) {
    int count = 0;
    int store_count = 0;
    uint16_t* temp_infor = new uint16_t[switches*1000];
    FILE* ofs;
    FILE* ofs_len;
    uint16_t** store_graph_info = new uint16_t*[report_inter];
    vector<uint16_t> store_info_len;
    if(if_store){
        string file_path("data/all_graph_route_ksp/" + store_file + "/" + store_file + to_string(thread_label));
        string len_path(file_path + "_num");
        ofs = fopen(file_path.c_str(), "w");
        ofs_len = fopen(len_path.c_str(), "w");
    }
    for(int i = 0; i < route_pairs.size(); i++){
        uint16_t data_num  = search_up_down_ksp(route_pairs[i][0], route_pairs[i][1], path_num, vc_num, temp_infor, thread_label);
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


void Fc_tagger_kspz::pthread_up_down_ksp(int thread_num, int path_num, int vc_num, bool if_report, int report_inter, bool if_store){
    if(topo_index == NULL){
        cout << "Please generate topology!" << endl;
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

    graph_pr = new Graph*[thread_num];
    for(int i = 0; i < thread_num; i++){
        graph_pr[i] = new Graph(topo_path);
    }
    thread_ecmp_num = new int[thread_num];
    memset(thread_ecmp_num, 0, sizeof(int)*thread_num);
    ecmp_num = 0;

    string file_dir_name("");
    if(if_store){
        file_dir_name += gene_path_for_file_ksp("data/all_graph_route_ksp/", path_num, vc_num);
    }
    thread* th = new thread[thread_num];
    for(int i = 0; i < thread_num; i++){
        th[i] = thread(&Fc_tagger_kspz::thread_up_down_ksp, this, thread_pairs[i], i, path_num, vc_num, if_report, report_inter, if_store, file_dir_name);
    }
    for(int i = 0; i < thread_num; i++)
        th[i].join();

    string file_path("data/all_graph_route_ksp/" + file_dir_name + "/" + file_dir_name);
    string len_path(file_path + "_num"); 
    int state;
    FILE* ofs = fopen(file_path.c_str(), "w");
    FILE* ofs_len = fopen(len_path.c_str(), "w");
    for(int i = 0; i < thread_num; i++){
        string in_file_path("data/all_graph_route_ksp/" + file_dir_name + "/" + file_dir_name + to_string(i));
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
    for(int i = 0; i < thread_num; i++){
        ecmp_num += thread_ecmp_num[i];
        delete graph_pr[i];
        graph_pr[i] = NULL;
    }
    cout << "Total number of ecmp:" << ecmp_num << endl;
    delete graph_pr;
    graph_pr = NULL;
    delete thread_ecmp_num;
    thread_ecmp_num = NULL;
}


double Fc_tagger_kspz::throughput_test_ksp(string type, int seed, int path_number, int vc_num){
    string file_dir_name("");
    file_dir_name += "sw";
    file_dir_name += to_string(switches);
    file_dir_name += "_vir";
    for(int i = 0; i < layer_num; i++)
        file_dir_name += to_string(vir_layer_degree[i]);
    file_dir_name += "_rand";
    file_dir_name += to_string(is_random*random_seed);
    file_dir_name += "_";
    file_dir_name += to_string(path_number);
    file_dir_name += "_";
    file_dir_name += to_string(vc_num);
    if(access(("data/all_graph_route_ksp/" + file_dir_name).c_str(), 0)){
        cout << "No route infor!" << endl;
        exit(1);
    }

    string file_path("data/all_graph_route_ksp/" + file_dir_name + "/" + file_dir_name);
    string len_path(file_path + "_num"); 
    int state;
    FILE* ofs = fopen(file_path.c_str(), "r");
    FILE* ofs_len = fopen(len_path.c_str(), "r");
    fseek(ofs_len, 0, SEEK_END);
    int len_size = ftell(ofs_len);
    rewind(ofs_len);
    fseek(ofs, 0, SEEK_END);
    int file_size = ftell(ofs);
    rewind(ofs);
    uint16_t *pair_len = new uint16_t[len_size/2];
    uint16_t *pair_infor = new uint16_t[file_size/2];
    state = fread(pair_len, sizeof(uint16_t), len_size/2, ofs_len);
    state = fread(pair_infor, sizeof(uint16_t), file_size/2, ofs);

    unordered_map<int, vector<int>> path_map_link;
    int node_len;
    int basic_len = 0;
    int node1, node2;
    int src = 0, dst = 1;
    int basic_count = 1;
    int sw1, sw2;
    int map_count;
    int layer1, layer2;
    int new_node1, new_node2;

    float *flow_matrix[switches];
    for(int i = 0; i < switches; i++){
        flow_matrix[i] = new float[switches];
        memset(flow_matrix[i], 0, sizeof(float)*switches);
    }
    if(type == "aa"){
        float pair_flow = hosts/float(switches-1);
        for(int i = 0; i < switches; i++)
            for(int j = 0; j < switches; j++)
                if(i != j)
                    flow_matrix[i][j] = pair_flow;
    }
    else if(type == "ur")
        gene_uniform_random(flow_matrix, seed);
    else if(type == "wr")
        gene_worse_case(flow_matrix);

    int max_through = 10;
    GRBEnv *env = new GRBEnv();
    GRBModel model = GRBModel(*env);
    GRBVar **flow_var;
    flow_var = new GRBVar*[switches*switches];
    int *path_num[switches];
    for(int i = 0; i < switches; i++){
        path_num[i] = new int[switches];
    }
    GRBVar throughput = model.addVar(0, max_through, 0, GRB_CONTINUOUS, "throughput");
    int path_sum = 0;
    for(int i = 0; i < switches*(switches-1)/2; i++){
        node_len = pair_len[2*i+1];
        int count = 0;
        int path_count = 0;
        path_sum += pair_len[2*i];
        if(flow_matrix[src][dst] > 0){
            flow_var[src*switches+dst] = model.addVars(pair_len[2*i], GRB_CONTINUOUS);
            path_num[src][dst] = pair_len[2*i];
            for(int j = 0; j < pair_len[2*i]; j++){
                flow_var[src*switches+dst][j].set(GRB_DoubleAttr_LB, 0.0);
                flow_var[src*switches+dst][j].set(GRB_DoubleAttr_UB, 100);
            }
        }
        if(flow_matrix[dst][src] > 0){
            flow_var[dst*switches+src] = model.addVars(pair_len[2*i], GRB_CONTINUOUS);
            path_num[dst][src] = pair_len[2*i];
            for(int j = 0; j < pair_len[2*i]; j++){
                flow_var[dst*switches+src][j].set(GRB_DoubleAttr_LB, 0.0);
                flow_var[dst*switches+src][j].set(GRB_DoubleAttr_UB, 100);
            }
        }
        while(count < node_len - 1){
            node1 = pair_infor[basic_len+count];
            node2 = pair_infor[basic_len+count+1];
            sw1 = node1%switches;
            sw2 = node2%switches;
            layer1 = node1/switches;
            layer2 = node2/switches;
            if(sw1 != sw2){
                if(flow_matrix[src][dst] > 0){
                    map_count = node1*(2*layer_num-1)*switches+node2;
                    if(path_map_link.find(map_count) == path_map_link.end()){
                        vector<int> temp = {src*switches+dst, path_count};
                        path_map_link[map_count] = temp;
                    }
                    else{
                        path_map_link[map_count].push_back(src*switches+dst);
                        path_map_link[map_count].push_back(path_count);
                    }
                }
                if(flow_matrix[dst][src] > 0){
                    new_node1 = node1 + (layer_num-1-layer1)*2*switches;
                    new_node2 = node2 + (layer_num-1-layer2)*2*switches;
                    map_count = new_node2*(2*layer_num-1)*switches+new_node1;
                    if(path_map_link.find(map_count) == path_map_link.end()){
                        vector<int> temp = {dst*switches+src, path_count};
                        path_map_link[map_count] = temp;
                    }
                    else{
                        path_map_link[map_count].push_back(dst*switches+src);
                        path_map_link[map_count].push_back(path_count);
                    }
                }
            }
            if(sw2 == dst){
                count++;
                path_count++;
            }
            count++;
        }
        dst++;
        if(dst == switches){
            src = basic_count;
            dst = ++basic_count;
        }
        basic_len += node_len;
    }
    fclose(ofs);
    fclose(ofs_len);
    cout << "Average path num: " << float(path_sum)*2/(switches*(switches-1)) << endl; 
  
    for(int i = 0; i < switches; i++){
        for(int j = 0; j < switches; j++){
            if(i != j){
                if(flow_matrix[i][j] > 0){
                    GRBLinExpr constr = 0;
                    for (int k = 0; k < path_num[i][j]; k++) 
                        constr += flow_var[i*switches+j][k];
                    constr -= throughput * flow_matrix[i][j];
                    model.addConstr(constr, GRB_EQUAL, 0);
                }
            }
        }
    }  
    for(auto &link : path_map_link){
        GRBLinExpr constr = 0;
        for(int i = 0; i < link.second.size()/2; i++){
            constr += flow_var[link.second[2*i]][link.second[2*i+1]];
        }
        model.addConstr(constr, GRB_LESS_EQUAL, 1);
    }
    cout << "All consts added" << endl;

    model.setObjective(1 * throughput, GRB_MAXIMIZE);
    model.set(GRB_IntParam_OutputFlag, 0);
    double throught_result = 0.0;
    try {
        model.optimize();
        if(model.get(GRB_IntAttr_Status) != GRB_OPTIMAL)
            cout << "Not optimal " << endl;
        else{
            throught_result = model.get(GRB_DoubleAttr_ObjVal);
            cout << "Throughtput: " << throught_result << endl;
        }
    } catch (GRBException e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    }

    delete[] pair_len;
    delete[] pair_infor;
    for(int i = 0; i < switches; i++){
        delete[] path_num[i];
    }
    delete[] flow_var;
    return throught_result;
}
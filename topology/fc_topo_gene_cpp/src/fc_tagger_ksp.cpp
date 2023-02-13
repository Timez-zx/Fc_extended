#include "fc_tagger_ksp.h"


void Fc_tagger_ksp::save_graph_infor(){
    if(topo_index == NULL){
        cout << "Please generate graph infor first by fc_topo_gene_1v1!" << endl;
        exit(1);
    }
    string file_dir_name = gene_path_for_file("data/topo_infor/");
    ofstream ofs("data/topo_infor/" + file_dir_name + "/" + file_dir_name+".txt");
    int degree;
    int basic_index = 0;
    int src, dst;
    ofs << switches << endl;
    for(int i = 0; i < layer_num-1; i++){
        degree = bipart_degree[i];
        for(int j = 0; j < switches; j++){
            src = j;
            for(int k = 1; k < degree; k++){
                dst = topo_index[basic_index+j*degree+k];
                link_to_layer_map[src*switches+dst] = (layer_num-i-1)*layer_num+layer_num-i-2;
                link_to_layer_map[dst*switches+src] = (layer_num-i-2)*layer_num+layer_num-i-1;
                ofs << src << " " << dst << " " << 1 << endl;
                ofs << dst << " " << src << " " << 1 << endl;
            }
        }
        
        basic_index += degree*switches;
    }
    ofs.close();
    my_graph = new Graph("data/topo_infor/" + file_dir_name + "/" + file_dir_name+".txt");
}


uint16_t Fc_tagger_ksp::search_up_down_ksp(int src, int dst, int path_num, int vc_num, uint16_t *path_infor){
    YenTopKShortestPathsAlg yenAlg(*my_graph, (*my_graph).get_vertex(src), (*my_graph).get_vertex(dst));
	int i = 0;
    int path[1000];
    int layer_pass[1000];
    int path_len;
    int src_inter, dst_inter, src_layer, dst_layer;
    int past_layer = 0;
    int vc_used;
    uint16_t real_path[1000];
    uint16_t data_num = 0;
	while(yenAlg.has_next() & i < path_num){
		yenAlg.next()->get_path(path, &path_len);
        vc_used = 1;
        past_layer = 0;
        for(int i = 0; i < path_len-1; i++){
            src_inter = path[i];
            dst_inter = path[i+1];
            src_layer = link_to_layer_map[src_inter*switches+dst_inter]/layer_num;
            dst_layer = link_to_layer_map[src_inter*switches+dst_inter]%layer_num;
            layer_pass[2*i] = src_layer;
            layer_pass[2*i+1] = dst_layer;
        }
        for(int i = 0; i < 2*(path_len-1)-1; i++){
            src_layer = layer_pass[i];
            if(src_layer < past_layer && src_layer < layer_pass[i+1]){
                vc_used++;
            }
            if(src_layer > past_layer)
                past_layer = src_layer;
        }


        if(vc_used > vc_num)
            continue;
        else{
            i++;
            // for(int i = 0; i < 2*(path_len-1); i++){
            //     cout << layer_pass[i] << " ";
            // }
            // cout << endl;
            // for(int i = 0; i < path_len; i++){
            //     cout << path[i] << " ";
            // }
            // cout << endl;
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



void Fc_tagger_ksp::thread_up_down_ksp(vector<int*> route_pairs, int thread_label, int path_num, int vc_num, bool if_report, int report_inter, bool if_store, string store_file) {
    int count = 0;
    int store_count = 0;
    uint16_t* temp_infor = new uint16_t[switches*1000];
    FILE* ofs;
    FILE* ofs_len;
    uint16_t** store_graph_info = new uint16_t*[report_inter];
    vector<uint16_t> store_info_len;
    if(if_store){
        string file_path("data/all_graph_route/" + store_file + "/" + store_file + to_string(thread_label));
        string len_path(file_path + "_num");
        ofs = fopen(file_path.c_str(), "w");
        ofs_len = fopen(len_path.c_str(), "w");
    }
    for(int i = 0; i < route_pairs.size(); i++){
        uint16_t data_num  = search_up_down_ksp(route_pairs[i][0], route_pairs[i][1], path_num, vc_num, temp_infor);
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


void Fc_tagger_ksp::pthread_up_down_ksp(int thread_num, int path_num, int vc_num, bool if_report, int report_inter, bool if_store){
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

    string file_dir_name("");
    if(if_store){
        file_dir_name += gene_path_for_file("data/all_graph_route/");
    }
    thread* th = new thread[thread_num];
    for(int i = 0; i < thread_num; i++){
        th[i] = thread(&Fc_tagger_ksp::thread_up_down_ksp, this, thread_pairs[i], i, path_num, vc_num, if_report, report_inter, if_store, file_dir_name);
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
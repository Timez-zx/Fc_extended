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


void Fc_tagger_ksp::search_up_down_ksp(int src, int dst, int path_num, int vc_num){
    YenTopKShortestPathsAlg yenAlg(*my_graph, (*my_graph).get_vertex(src), (*my_graph).get_vertex(dst));
	int i = 0;
    int path[1000];
    int layer_pass[1000];
    int path_len;
    int src_inter, dst_inter, src_layer, dst_layer;
    int past_layer = 0;
    int vc_used;
    int real_path[1000];
	while(yenAlg.has_next() & i < path_num)
	{
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
            }
        }
        
	}
}
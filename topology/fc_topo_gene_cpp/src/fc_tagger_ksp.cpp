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
	while(yenAlg.has_next() & i < path_num)
	{
		++i;
		yenAlg.next()->PrintOut(cout);
	}
}
#include "fc_base.h"


Fc_base::~Fc_base(){
    if(bipart_degree){
        delete[] bipart_degree;
        bipart_degree = NULL;
    }
    if(topo_index){
        delete[] topo_index;
        topo_index = NULL;
    }
    if (bit_map)
    {
        for(int i = 0; i < switches; i++){
            if(bit_map[i]){
                delete[] bit_map[i];
                bit_map[i] = NULL;
            }
        }   
        delete[] bit_map;
        bit_map = NULL;
    }    
}


void Fc_base::fc_topo_gene_1v1(int fast_or_not){
    int sw_ports = ports - hosts;
    int bipart_num = layer_num - 1;
    int degree;
    int src, dst;
    int initial_sub = 0;
    bipart_degree = new int[bipart_num+1];
    int bipart_count = 0;
    int total_degree = 0;
    int remain_degree = vir_layer_degree[layer_num-1];
    bit_map = new int*[switches];
    for(int i = 0; i < switches; i++){
        bit_map[i] = new int[switches];
        memset(bit_map[i], 0, sizeof(int)*switches);
    }
    total_degree += remain_degree + 1;
    for(int i = layer_num-2; i > 0; i--){
        remain_degree = vir_layer_degree[i] - remain_degree;
        total_degree += remain_degree + 1;
    }
    topo_index = new int[switches*total_degree];
    bool **vertex_check = new bool*[switches];
    for(int i = 0; i < switches; i++){
        vertex_check[i] = new bool[switches];
        for(int j = 0; j < switches; j++)
            vertex_check[i][j] = false;
    }
    vector<vector<int> > poss_connect;
    int poss_connect_num[switches];
    for(int i = 0; i < switches; i++){
        poss_connect_num[i] = switches-1;
        vector<int> temp;
        for(int j = 0; j < switches; j++){
            if(i != j)
                temp.push_back(j);
        }
        poss_connect.push_back(temp);
    }
    if(is_random)
        srand(random_seed);

    int two_count, zero_count;
    int index_basic = 0;
    int rand_index;
    int degrees[switches];
    int degree_label[switches];
    for(int i = layer_num - 1; i > 0; i--){
        degree = vir_layer_degree[i] - initial_sub;
        bipart_degree[bipart_count] = degree + 1;
        bipart_count++;
        for(int j = 0; j < switches; j++){
            topo_index[index_basic+j*(degree+1)] = j;
        }
        for(int j = 0; j < degree; j++){
            zero_count = switches;
            two_count = 0;
            memset(degrees, 0, sizeof(int)*switches);
            memset(degree_label, 0, sizeof(int)*switches);
            vector<int> src_choose(switches);
            int src_remain = switches;
            for(int k = 0; k < switches; k++)
                src_choose[k] = k;
            src = 0;
            while(two_count < switches){
                int count_break = 0;
                rand_index = rand()%poss_connect_num[src];
                dst = poss_connect[src][rand_index];
                degrees[src]++;
                degrees[dst]++;
                if(degrees[src] == 1)
                    zero_count--;
                if(degrees[dst] == 1)
                    zero_count--;
                if(degrees[src] == 2)
                    two_count++;
                if(degrees[dst] == 2)
                    two_count++;
                while(vertex_check[src][dst] == true || (two_count+zero_count == switches && zero_count < 3 && src_remain != 1) || degree_label[dst] == 1){
                    if(degrees[dst] == 2)
                        two_count--;
                    if(degrees[dst] == 1)
                        zero_count++;
                    degrees[dst]--;
                    rand_index = rand()%poss_connect_num[src];
                    dst = poss_connect[src][rand_index];
                    degrees[dst]++;
                    if(count_break > 1e6){
                        cout << "Can't construct, please change the rand seed or use other mode!" << endl;
                        exit(1);
                    }
                    count_break++;
                    if(degrees[dst] == 1)
                        zero_count--;
                    if(degrees[dst] == 2)
                        two_count++;
                }

                topo_index[index_basic+src*(degree+1)+j+1] = dst;
                vertex_check[src][dst] = true;
                vertex_check[dst][src] = true;
                bit_map[src][dst] = 1;
                bit_map[dst][src] = 1;
                degree_label[dst] = 1;
                remove(poss_connect[src].begin(), poss_connect[src].begin()+poss_connect_num[src], dst);
                remove(poss_connect[dst].begin(), poss_connect[dst].begin()+poss_connect_num[dst], src);
                poss_connect_num[src] -= 1;
                poss_connect_num[dst] -= 1;

                // cout << src << "->" << dst << endl;
                remove(src_choose.begin(), src_choose.begin()+src_remain, src);
                src_remain--;
                if(fast_or_not)
                    src++;
                else{
                    int mininum = switches+1;
                    for(int k = 0; k < src_remain; k++){
                        int temp_count = 0;
                        for(int m = 0; m < switches; m++){
                            temp_count += degree_label[m] | vertex_check[src_choose[k]][m];
                        }
                        temp_count = switches - temp_count;
                        if(temp_count <= mininum){
                            src = src_choose[k];
                            mininum = temp_count;
                        }
                    }
                }
            }
        }
        // cout << endl;
        initial_sub = degree;
        index_basic += (degree+1)*switches;
    }
    bipart_degree[bipart_count] = 0;
    for(int i = 0; i <  switches; i++){
        delete[] vertex_check[i];
    }
    delete[] vertex_check;
    cout << "Topology constructed" << endl;
}


string Fc_base::gene_path_for_file(string path){
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
    if(access((path + file_dir_name).c_str(), 0)){
        string cmd("mkdir ");
        cmd += path;
        cmd += file_dir_name;
        int temp = system(cmd.c_str());
    }
    return file_dir_name;
}


double Fc_base::throughput_test(string type, int seed){
    string file_dir_name("");
    file_dir_name += "sw";
    file_dir_name += to_string(switches);
    file_dir_name += "_vir";
    for(int i = 0; i < layer_num; i++)
        file_dir_name += to_string(vir_layer_degree[i]);
    file_dir_name += "_rand";
    file_dir_name += to_string(is_random*random_seed);
    if(access(("data/all_graph_route/" + file_dir_name).c_str(), 0)){
        cout << "No route infor!" << endl;
        exit(1);
    }

    string file_path("data/all_graph_route/" + file_dir_name + "/" + file_dir_name);
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


void Fc_base::gene_uniform_random(float **flow_matrix, int seed){
    int combination = switches/8;
    int rand_rate[combination];
    float rate[combination];
    int sum = 0;
    srand(seed);
    for(int i = 0; i < combination; i++){
        rand_rate[i] = rand()%10000;
        sum += rand_rate[i];
    }
    for(int i = 0; i < combination; i++)
        rate[i] = rand_rate[i]/float(sum);
    for(int i = 0; i < switches; i++)
        for(int j = 0; j < switches; j++)
            flow_matrix[i][j] = 0;
    for(int k = 0; k < combination; k++){
        int permu[switches];
        for(int i = 0; i < switches; i++){
            permu[i] = i;
        }
        shuffle(permu, permu+switches, default_random_engine((k+1)*seed));
        int store_same;
        int store_len = 0;
        for(int i = 0; i < switches; i++){
            if(permu[i] == i){
                if(store_len == 0){
                    store_same = i;
                    store_len++;
                }
                else{
                    permu[i] = store_same;
                    permu[store_same] = i;
                    store_len--;
                }
            }
        }
        if(store_len == 1){
            int rand_pick = rand()%switches;
            while(rand_pick == store_same)
                rand_pick = rand()%switches;
            permu[rand_pick] = store_same;
            permu[store_same] = rand_pick;
        }
        for(int i = 0; i < switches; i++)
            flow_matrix[i][permu[i]] += rate[k]*hosts;
    }
}


void Fc_base::gene_worse_case(float **flow_matrix){
    if(access("data/worst_flow_infor", 0)){
        string cmd("mkdir ");
        cmd += "data/worst_flow_infor";
        int temp = system(cmd.c_str());
    }
    string store_file("temp_infor");
    string file_path("data/worst_flow_infor/" + store_file);
    FILE* ofs = fopen(file_path.c_str(), "w");
    fwrite(&switches, sizeof(int), 1, ofs);
    fwrite(&hosts, sizeof(int), 1, ofs);
    for(int i = 0; i < switches; i++)
        fwrite(bit_map[i], sizeof(int), switches, ofs);
    fclose(ofs);
    string cmd("python3 src/worst_case.py");
    int state = system(cmd.c_str());

    string read_file("temp_infor_flow");
    string read_path("data/worst_flow_infor/" + read_file);
    ifstream ifs(read_path.c_str(), ios::in);
    for(int i = 0; i < switches; i++){
        for(int j = 0; j < switches; j++){
            ifs >> flow_matrix[i][j];
        }
    }
    ifs.close();
}


double Fc_base::get_fiber_cost(int fiber_len){
    int increaseCount;
    if(fiber_len <= 1)
        return 48.5/12;
    else if(fiber_len > 1 and fiber_len <= 2)
        return 51.5/12;
    else if(fiber_len > 2 and fiber_len <= 3)
        return 53.5/12;
    else if(fiber_len > 3 and fiber_len <= 5)
        return 56.5/12;
    else if(fiber_len > 5 and fiber_len <= 10)
        return 63.5/12;
    else if(fiber_len > 10 and fiber_len <= 15)
        return 68.5/12;
    else if(fiber_len > 15 and fiber_len <= 20)
        return 76.5/12;
    else if(fiber_len > 20 and fiber_len <= 30)
        return 88.5/12;
    else if(fiber_len > 30 and fiber_len <= 50)
        return 113.5/12;
    else if(fiber_len > 50 and fiber_len <= 100)
        return 198.5/12;
    else{
        increaseCount = (fiber_len-100)/50+1;
        return (198.5/12)+150.0/(12*99)*50*increaseCount;
    }

}


int Fc_base::cost_model(int ocs_ports, int* distance_infor, int column_num){
    if(topo_index == NULL){
        cout << "Please generate topology!" << endl;
        exit(1);
    }
    int ocs_number = ceil(switches*(ports-hosts)/float(ocs_ports));
    int tranceiver_num = 0;
    int basic_index = 0;
    int degree;
    int src, dst;
    int src_x, src_y;
    int dst_x, dst_y;
    int ocs_port_count = 0;
    int ocs_label = 0;
    int ocs_y = 0;
    int fiber_len_src, fiber_len_dst;
    int fiber_len = 0;
    double total_cost = 0;
    total_cost += 59099*switches;
    total_cost += 60000*ocs_number;
    total_cost += switches*hosts*189;
    for(int i = 0; i < layer_num-1; i++){
        degree = bipart_degree[i];
        for(int j = 0; j < switches; j++){
            src = j;
            src_x = ((src/(column_num*2))*distance_infor[0]+distance_infor[2]);
            src_y = (src % column_num)*distance_infor[1];
            for(int k = 1; k < degree; k++){
                dst = topo_index[basic_index+j*degree+k];
                dst_x = ((dst/(column_num*2))*distance_infor[0]+distance_infor[2]);
                dst_y = (dst % column_num)*distance_infor[1];
                fiber_len_src = src_x + abs(src_y-ocs_y);
                fiber_len_dst = dst_x + abs(dst_y-ocs_y);
                fiber_len += fiber_len_dst+fiber_len_src;
                tranceiver_num += 2;
                total_cost += get_fiber_cost(fiber_len_src+fiber_len_dst);
                if(ocs_port_count == ocs_ports - 2){
                    ocs_port_count = 0;
                    ocs_label++;
                    if(ocs_label%4 == 0)
                        ocs_y += distance_infor[3];
                }
                else
                    ocs_port_count += 2;
                
            }
        }
        basic_index += degree*switches;
    }
    total_cost += tranceiver_num*1100;
    cout << "The cabling length of Fc topo: "<< fiber_len << "m" << endl;
    cout << "The number of tranceiver: "<< tranceiver_num << endl;
    cout << "Total Cost: "<< ceil(total_cost) << endl;

    return fiber_len;
}


int Fc_base::bisection_bandwidth_byExchange(int random_seed, int cycle_times, int poss_base){
    if(topo_index == NULL){
        cout << "Please generate topology!" << endl;
        exit(1);
    }
    int index_basic[layer_num-1];
    index_basic[0] = 0;
    for(int i = 1; i < layer_num-1; i++)
        index_basic[i] = index_basic[i-1] + bipart_degree[i-1]*switches;

    srand(random_seed);
    int rand_switches[switches/2];
    int other_switches[switches/2];
    int rand_switches_label[switches];
    int rand_index_table[switches];
    int other_index_table[switches];

    memset(rand_switches_label, 0, switches*sizeof(int));
    memset(rand_index_table, 0xff, switches*sizeof(int));
    memset(other_index_table, 0xff, switches*sizeof(int));
    for(int i = 0; i < switches/2; i++){
        rand_switches[i] = rand()%switches;
        while(rand_switches_label[rand_switches[i]])
            rand_switches[i] = rand()%switches;
        rand_switches_label[rand_switches[i]] = 1;
        rand_index_table[rand_switches[i]] = i;
    }

    int other_count = 0;
    for(int i = 0; i < switches; i++){
        if(rand_switches_label[i] == 0){
            other_switches[other_count++] = i;
            other_index_table[other_switches[other_count-1]] = other_count-1;
        }
    }

    int rand_switch, other_switch;
    int degree;
    int dst;
    int bipart_band_rand[switches/2];
    int bipart_band_other[switches/2];
    int max_rand_switch, max_rand_index;
    int max_other_switch, max_other_index;
    int band;
    int min_band = 1e7;
    int equal_count = 0;
    int pick_count_other, pick_count_rand;
    int pick_rand[switches/2];
    int pick_other[switches/2];
    int rand_pick_index, other_pick_index;
    float possi;
    float compare;
    while(true){
        pick_count_other = 0;
        pick_count_rand = 0;
        band = 0;
        max_rand_index = 0;
        max_other_index = 0;
        max_other_switch = 0;
        max_rand_switch = 0;
        memset(bipart_band_rand, 0, switches/2*sizeof(int));
        memset(bipart_band_other, 0, switches/2*sizeof(int));
        for(int i = 0; i < switches/2; i++){
            rand_switch = rand_switches[i];
            for(int j = 0; j < layer_num-1; j++){
                degree = bipart_degree[j];
                for(int k = 1; k < degree; k++){
                    dst = topo_index[index_basic[j]+rand_switch*degree+k];
                    if(rand_switches_label[dst] == 0){
                        band++;
                        bipart_band_rand[i]++;
                        bipart_band_other[other_index_table[dst]]++;
                    }
                }
            }
        }

        for(int i = 0; i < switches/2; i++){
            other_switch = other_switches[i];
            for(int j = 0; j < layer_num-1; j++){
                degree = bipart_degree[j];
                for(int k = 1; k < degree; k++){
                    dst = topo_index[index_basic[j]+other_switch*degree+k];
                    if(rand_switches_label[dst] == 1){
                        band++;
                        bipart_band_other[i]++;
                        bipart_band_rand[rand_index_table[dst]]++;
                    }
                }
            }
        }
        if(band < min_band){
            min_band = band;
            equal_count = 0;
        }
        else if(band == min_band){
            equal_count++;
            if(equal_count > cycle_times)
                break;
        }

        for(int i = 0; i < switches/2; i++){
            if(bipart_band_rand[i] > max_rand_switch){
                max_rand_switch = bipart_band_rand[i];
                max_rand_index = i;
            }
            if(bipart_band_other[i] > max_other_switch){
                max_other_switch = bipart_band_other[i];
                max_other_index = i;
            }
        }

        for(int i = 0; i < switches/2; i++){
            if(bipart_band_rand[i] == max_rand_switch)
                pick_rand[pick_count_rand++] = i;
            if(bipart_band_other[i] == max_other_switch)
                pick_other[pick_count_other++] = i;
        }
        
        rand_pick_index = rand()%pick_count_rand;
        other_pick_index = rand()%pick_count_other;
        max_rand_index = pick_rand[rand_pick_index];
        max_other_index = pick_other[other_pick_index];
        
        possi = equal_count/float(cycle_times);
        compare = (rand()%100000+1)/float(100000);
        if(compare < possi/poss_base){
            max_rand_index = rand()%(switches/2);
            max_other_index = rand()%(switches/2);
        }

        rand_switches_label[rand_switches[max_rand_index]] = 0;
        rand_switches_label[other_switches[max_other_index]] = 1;
        rand_index_table[rand_switches[max_rand_index]] = -1;
        other_index_table[other_switches[max_other_index]] = -1;
        rand_index_table[other_switches[max_other_index]] = max_rand_index;
        other_index_table[rand_switches[max_rand_index]] = max_other_index;
        int temp = rand_switches[max_rand_index];
        rand_switches[max_rand_index] = other_switches[max_other_index];
        other_switches[max_other_index] = temp;
    }
    return min_band;
}


void Fc_base::b_bandwidth_onethread(int thread_label, int rand_interval, int cycle_times, int* band_find, int poss_base){
    int min_band = 1e7;
    int band;
    for(int i = thread_label*rand_interval; i < (thread_label+1)*rand_interval; i++){
        band = bisection_bandwidth_byExchange(i, cycle_times, poss_base);
        if(band < min_band)
            min_band = band;
    }
    *band_find = min_band;
    cout << "Thread " << thread_label << ":" << min_band << endl;
}


void Fc_base::multi_thread_b_bandwidth(int thread_num, int rand_interval, int cycle_times, int poss_base){
    thread* th = new thread[thread_num];
    int band_find[thread_num];
    for(int i = 0; i < thread_num; i++){
        th[i] = thread(&Fc_base::b_bandwidth_onethread, this, i, rand_interval, cycle_times, band_find+i, poss_base);
    }
    for(int i = 0; i < thread_num; i++)
        th[i].join();
    int min_band = 1e7;
    for(int i = 0; i < thread_num; i++){
        if(band_find[i] < min_band)
            min_band = band_find[i];
    }
    cout << "Min band: " << min_band << endl;
}

#include <iostream>
#include <vector>
using namespace std;

class Fc_topo_all_route{
    public:
        // Initial part
        int switches;
        int hosts;
        int ports;
        int* vir_layer_degree;
        int layer_num;
        int is_random;
        int random_seed;

        Fc_topo_all_route(int switches, int hosts, int ports, int* vir_layer_degree, int layer_num, int is_random, int random_seed):
        switches(switches), hosts(hosts), ports(ports), vir_layer_degree(vir_layer_degree), layer_num(layer_num),is_random(is_random), random_seed(random_seed){}
        // generate topology
        void fc_topo_gene(void);



};

void Fc_topo_all_route::fc_topo_gene(void){
    int sw_ports = ports - hosts;
    int bipart_num = layer_num - 1;
    int* switch_array = new int(switches);
    int degree;
    int* degrees = new int(switches);
    int basic;

    for(int i = 0; i < switches; i++){
        switch_array[i] = i;
    }

    for(int i = layer_num - 1; i > 0; i--){
        degree = vir_layer_degree[i];
        for(int j = 0; j < switches; j++){
            degrees[j] = degree;
        }
        basic = switches - 1;
        
    }
    
}

int main(){
    int switches = 5;
    int hosts = 24;
    int ports = 36;
    int vir_layer_degree[] = {2, 4, 4, 2};
    int layer_num = 4;
    int is_random = 1;
    int random_seed = 1;
    Fc_topo_all_route fc_test(switches, hosts, ports, vir_layer_degree, layer_num, is_random, random_seed);
    fc_test.fc_topo_gene();
    return 0;
}
#include "fc_tagger_test.h"

void FcTaggerTest::SaveTaggerGraph(){
    if(topo_index == NULL){
        cout << "Please generate graph infor first by fc_topo_gene_1v1!" << endl;
        exit(1);
    }
    int degree, src, dst, basicIndex = 0;
    int *portLabel = new int[switches]();
    for(int i = 0; i < layer_num-1; i++){
        degree = bipart_degree[i];
        for(int j = 0; j < switches; j++){
            src = j;
            for(int k = 1; k < degree; k++){
                dst = topo_index[basicIndex+j*degree+k];
                linkPortMap[src*switches+dst] = portLabel[src]*(ports-hosts)+portLabel[dst];
                portLabel[src]++;
                portLabel[dst]++;
            }
        }
        basicIndex += degree*switches;
    }
    delete portLabel;
    portLabel = NULL;
}
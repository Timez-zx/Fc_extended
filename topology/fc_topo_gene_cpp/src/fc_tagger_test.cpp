#include "fc_tagger_test.h"

void FcTaggerTest::SaveTaggerGraph(){
    if(topo_index == NULL){
        cout << "Please generate graph infor first by fc_topo_gene_1v1!" << endl;
        exit(1);
    }
    string fileDirName = gene_path_for_file("data/topo_infor/");
    ofstream ofs("data/topo_infor/" + fileDirName + "/" + fileDirName+".txt");
    int degree, src, dst, basicIndex = 0;
    int *portLabel = new int[switches]();
    ofs << "LinkID,SourceID,DestinationID,PeerID,Cost,Bandwidth,Delay,SRLGNum,SRLGs" << endl;
    int count = 0;
    for(int i = 0; i < layer_num-1; i++){
        degree = bipart_degree[i];
        for(int j = 0; j < switches; j++){
            src = j;
            for(int k = 1; k < degree; k++){
                dst = topo_index[basicIndex+j*degree+k];
                linkPortMap[src*switches+dst] = portLabel[src]*(ports-hosts)+portLabel[dst];
                portLabel[src]++;
                portLabel[dst]++;
                ofs << count << "," <<src << "," << dst << "," << 0 << "," << 1 << ",0,0,0,0"<< endl;
                count++;
                ofs << count << "," <<dst << "," << src << "," << 0 << "," << 1 << ",0,0,0,0"<< endl;
                count++;
            }
        }
        basicIndex += degree*switches;
    }
    ofs.close();
    delete portLabel;
    portLabel = NULL;
    topoPath = "data/topo_infor/" + fileDirName + "/" + fileDirName+".txt";
}


uint16_t FcTaggerTest::SearchKsp(int srcIn, int dstIn, int pathNum, int thLabel, uint16_t *pathInfor){
    KShortestPath ksp(graphPr[thLabel]);
    double cost = ksp.Init(srcIn, dstIn);
    int pathCount = 0, pathLen = 0;
    while(cost < 10000 & pathCount < pathNum){
        vector<Link*> pathTemp = ksp.GetPath();
        for(int i = 0; i < pathTemp.size(); i++){
            pathInfor[pathLen] = pathTemp[i]->source_id;
            pathLen++;
        }
        pathInfor[pathLen] = dstIn;
        pathLen++;
        pathNum++;
        cost = ksp.FindNextPath();
    }
    return pathLen;
}


uint16_t FcTaggerTest::SearchEcmp(int srcIn, int dstIn, int thLabel, uint16_t *pathInfor){
    KShortestPath ksp(graphPr[thLabel]);
    double cost = ksp.Init(srcIn, dstIn);
    double minCost = cost;
    int pathLen = 0;
    while(cost < 10000 & cost == minCost){
        vector<Link*> pathTemp = ksp.GetPath();
        for(int i = 0; i < pathTemp.size(); i++){
            pathInfor[pathLen] = pathTemp[i]->source_id;
            pathLen++;
        }
        pathInfor[pathLen] = dstIn;
        pathLen++;
        pathNum++;
        cost = ksp.FindNextPath();
    }
    return pathLen;
}


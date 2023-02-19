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
                linkPortMap[dst*switches+src] = portLabel[dst]*(ports-hosts)+portLabel[src];
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


uint16_t FcTaggerTest::SearchKsp(int srcIn, int dstIn, int pathNum, int thLabel, int *pathInfor){
    KShortestPath ksp(graphPr[thLabel]);
    double cost = ksp.Init(srcIn, dstIn);
    int pathCount = 0, pathLen = 0;
    int tempDst, tempSrc, tempLen;
    while(cost < 10000 & pathCount < pathNum){
        vector<Link*> pathTemp = ksp.GetPath();
        tempLen = pathTemp.size();
        pathInfor[pathLen] = 0*switches*(ports-hosts+1)+srcIn*(ports-hosts+1)+ports-hosts;
        pathLen++; 
        for(int i = 0; i < tempLen-1; i++){
            tempSrc = pathTemp[i]->source_id;
            tempDst = pathTemp[i]->ter_id;
            pathInfor[pathLen] = (i+1)*switches*(ports-hosts+1)+tempDst*(ports-hosts+1)+linkPortMap[tempSrc*switches+tempDst]%(ports-hosts);
            pathInfor[pathLen+1] = pathInfor[pathLen]; 
            pathLen += 2;
        }
        tempSrc = pathTemp[tempLen-1]->source_id;
        tempDst = pathTemp[tempLen-1]->ter_id;
        pathInfor[pathLen] = tempLen*switches*(ports-hosts+1)+tempDst*(ports-hosts+1)+linkPortMap[tempSrc*switches+tempDst]%(ports-hosts);
        pathLen++;

        pathInfor[pathLen] = 0*switches*(ports-hosts+1)+dstIn*(ports-hosts+1)+ports-hosts;
        pathLen++; 
        for(int i = tempLen-1; i > 0; i--){
            tempSrc = pathTemp[i]->ter_id;
            tempDst = pathTemp[i]->source_id;
            pathInfor[pathLen] = (tempLen-i)*switches*(ports-hosts+1)+tempDst*(ports-hosts+1)+linkPortMap[tempSrc*switches+tempDst]%(ports-hosts);
            pathInfor[pathLen+1] = pathInfor[pathLen]; 
            pathLen += 2;
        }
        tempSrc = pathTemp[0]->ter_id;
        tempDst = pathTemp[0]->source_id;
        pathInfor[pathLen] = tempLen*switches*(ports-hosts+1)+tempDst*(ports-hosts+1)+linkPortMap[tempSrc*switches+tempDst]%(ports-hosts);
        pathLen++; 
        pathNum++;
        cost = ksp.FindNextPath();
    }
    // for(int i = 0; i < pathLen; i++){
    //     cout << pathInfor[i] << " ";
    // }
    // cout << endl;
    return pathLen;
}


uint16_t FcTaggerTest::SearchEcmp(int srcIn, int dstIn, int thLabel, int *pathInfor){
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
        cost = ksp.FindNextPath();
    }
    return pathLen;
}


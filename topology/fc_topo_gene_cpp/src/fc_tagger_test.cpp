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
                // cout << src << " " << dst << " " << portLabel[src] << " " << portLabel[dst] << endl;
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


int FcTaggerTest::SearchKsp(int srcIn, int dstIn, int pathNum, int thLabel, int *pathInfor){
    KShortestPath ksp(graphPr[thLabel]);
    double cost = ksp.Init(srcIn, dstIn);
    int pathCount = 0, pathLen = 0;
    int tempDst, tempSrc, tempLen;
    while(cost < 10000 & pathCount < pathNum){
        if(cost + 1 > thTagNum[thLabel])
            thTagNum[thLabel] = cost+1;
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
        pathCount++;
        cost = ksp.FindNextPath();
    }
    return pathLen;
}


void FcTaggerTest::threadKsp(vector<int*> routePairs, int thLabel, int pathNum, bool ifReport, int reportInter, bool ifStore, string storeFile){
    FILE* ofs;
    int dataNum;
    int count = 0, storeCount = 0;
    int* tempInfor = new int[switches*10000];
    int** storeGraphInfo = new int*[reportInter];
    vector<int> storeInfoLen;
    if(ifStore){
        string filePath("data/tagger_infor/" + storeFile + "/" + storeFile + to_string(thLabel));
        ofs = fopen(filePath.c_str(), "w");
    }
    for(int i = 0; i < routePairs.size(); i++){
        dataNum  = SearchKsp(routePairs[i][0], routePairs[i][1], pathNum, thLabel, tempInfor);
        if(ifReport){
            count++;
            if(count % reportInter == 0){
                cout << "The thread " << thLabel << " " <<count/double(routePairs.size()) << endl;
            }
        }
        if(ifStore){
            int *tempData = new int[dataNum];
            memcpy(tempData, tempInfor, sizeof(int)*dataNum);
            storeGraphInfo[storeCount] = tempData;
            storeInfoLen.push_back(dataNum);
            storeCount++;
            if(storeCount == reportInter) {
                for(int j = 0; j < reportInter; j++){
                    fwrite(storeGraphInfo[j], sizeof(int), storeInfoLen[j], ofs);
                    delete[] storeGraphInfo[j];
                    storeGraphInfo[j] = NULL;
                }
                fflush(ofs);
                storeInfoLen.clear();
                storeCount = 0;
            }
        }
    }
    if(ifStore){
        for(int j = 0; j < storeCount; j++){
            fwrite(storeGraphInfo[j], sizeof(int), storeInfoLen[j], ofs);
            delete[] storeGraphInfo[j];
        }
        fclose(ofs);
    }
    delete[] tempInfor;
    tempInfor = NULL;
    delete[] storeGraphInfo;
    storeGraphInfo = NULL;
}


void FcTaggerTest::mthreadKsp(int threadNum, int pathNum, bool ifReport, int reportInter, bool ifStore){
    if(topo_index == NULL){
        cout << "Please generate topology!" << endl;
        exit(1);
    }
    int totalPairs = switches*(switches-1)/2;
    int average = ceil(totalPairs/float(threadNum));
    int count = 0, alloIndex = 0;
    vector<vector<int*> > threadPairs;
    for(int i = 0; i < threadNum; i++){
        vector<int*> pairs;
        threadPairs.push_back(pairs);
    }
    int *temp = new int[2];
    for(int i = 0; i < switches; i++){
        for (int j = i+1; j < switches; j++){
            temp = new int[2];
            temp[0] = i;
            temp[1] = j;
            threadPairs[alloIndex].push_back(temp);
            if(count < average - 1)
                count++;
            else{
                count = 0;
                alloIndex++;
            }
        } 
    }

    graphPr = new Graph*[threadNum];
    thTagNum = new int[threadNum]();
    int maxTag = 0;
    for(int i = 0; i < threadNum; i++){
        graphPr[i] = new Graph(topoPath);
    }

    string fileDirName("");
    if(ifStore)
        fileDirName += gene_path_for_file("data/tagger_infor/");
    thread* th = new thread[threadNum];
    for(int i = 0; i < threadNum; i++){
        th[i] = thread(&FcTaggerTest::threadKsp, this, threadPairs[i], i, pathNum, ifReport, reportInter, ifStore, fileDirName);
    }
    for(int i = 0; i < threadNum; i++)
        th[i].join();

    string filePath("data/tagger_infor/" + fileDirName + "/" + fileDirName + "_ksp" + to_string(pathNum));
    FILE* ofs = fopen(filePath.c_str(), "w");
    fclose(ofs);
    int state;
    for(int i = 0; i < threadNum; i++){
        string inFilePath("data/tagger_infor/" + fileDirName + "/" + fileDirName + to_string(i));
        string cmd("cat ");
        cmd += inFilePath;
        cmd += " >> ";
        cmd += filePath;
        state = system(cmd.c_str());
        cmd = "rm " + inFilePath;
        state = system(cmd.c_str());
    }
    for(int i = 0; i < threadNum; i++){
        if(maxTag < thTagNum[i])
            maxTag = thTagNum[i];
        delete graphPr[i];
        graphPr[i] = NULL;
    }
    cout << maxTag << endl;
    ofs = fopen(filePath.c_str(), "a");
    fwrite(&maxTag, sizeof(int), 1, ofs);
    fclose(ofs);
    delete graphPr;
    graphPr = NULL;
    delete thTagNum;
    thTagNum = NULL;
}


int FcTaggerTest::SearchEcmp(int srcIn, int dstIn, int thLabel, int *pathInfor){
    KShortestPath ksp(graphPr[thLabel]);
    double cost = ksp.Init(srcIn, dstIn);
    double minCost = cost;
    int tempDst, tempSrc, tempLen;
    int pathLen = 0;
    while(cost < 10000 & cost == minCost){
        if(cost + 1 > thTagNum[thLabel])
            thTagNum[thLabel] = cost+1;
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
        cost = ksp.FindNextPath();
    }
    return pathLen;
}


void FcTaggerTest::threadEcmp(vector<int*> routePairs, int thLabel, bool ifReport, int reportInter, bool ifStore, string storeFile){
    FILE* ofs;
    int dataNum;
    int count = 0, storeCount = 0;
    int* tempInfor = new int[switches*10000];
    int** storeGraphInfo = new int*[reportInter];
    vector<int> storeInfoLen;
    if(ifStore){
        string filePath("data/tagger_infor/" + storeFile + "/" + storeFile + to_string(thLabel));
        ofs = fopen(filePath.c_str(), "w");
    }
    for(int i = 0; i < routePairs.size(); i++){
        dataNum  = SearchEcmp(routePairs[i][0], routePairs[i][1], thLabel, tempInfor);
        if(ifReport){
            count++;
            if(count % reportInter == 0){
                cout << "The thread " << thLabel << " " <<count/double(routePairs.size()) << endl;
            }
        }
        if(ifStore){
            int *tempData = new int[dataNum];
            memcpy(tempData, tempInfor, sizeof(int)*dataNum);
            storeGraphInfo[storeCount] = tempData;
            storeInfoLen.push_back(dataNum);
            storeCount++;
            if(storeCount == reportInter) {
                for(int j = 0; j < reportInter; j++){
                    fwrite(storeGraphInfo[j], sizeof(int), storeInfoLen[j], ofs);
                    delete[] storeGraphInfo[j];
                    storeGraphInfo[j] = NULL;
                }
                fflush(ofs);
                storeInfoLen.clear();
                storeCount = 0;
            }
        }
    }
    if(ifStore){
        for(int j = 0; j < storeCount; j++){
            fwrite(storeGraphInfo[j], sizeof(int), storeInfoLen[j], ofs);
            delete[] storeGraphInfo[j];
        }
        fclose(ofs);
    }
    delete[] tempInfor;
    tempInfor = NULL;
    delete[] storeGraphInfo;
    storeGraphInfo = NULL;
}


void FcTaggerTest::mthreadEcmp(int threadNum, bool ifReport, int reportInter, bool ifStore){
    if(topo_index == NULL){
        cout << "Please generate topology!" << endl;
        exit(1);
    }
    int totalPairs = switches*(switches-1)/2;
    int average = ceil(totalPairs/float(threadNum));
    int count = 0, alloIndex = 0;
    vector<vector<int*> > threadPairs;
    for(int i = 0; i < threadNum; i++){
        vector<int*> pairs;
        threadPairs.push_back(pairs);
    }
    int *temp = new int[2];
    for(int i = 0; i < switches; i++){
        for (int j = i+1; j < switches; j++){
            temp = new int[2];
            temp[0] = i;
            temp[1] = j;
            threadPairs[alloIndex].push_back(temp);
            if(count < average - 1)
                count++;
            else{
                count = 0;
                alloIndex++;
            }
        } 
    }

    graphPr = new Graph*[threadNum];
    thTagNum = new int[threadNum]();
    int maxTag = 0;
    for(int i = 0; i < threadNum; i++){
        graphPr[i] = new Graph(topoPath);
    }

    string fileDirName("");
    if(ifStore)
        fileDirName += gene_path_for_file("data/tagger_infor/");
    thread* th = new thread[threadNum];
    for(int i = 0; i < threadNum; i++){
        th[i] = thread(&FcTaggerTest::threadEcmp, this, threadPairs[i], i, ifReport, reportInter, ifStore, fileDirName);
    }
    for(int i = 0; i < threadNum; i++)
        th[i].join();

    string filePath("data/tagger_infor/" + fileDirName + "/" + fileDirName + "_ecmp");
    FILE* ofs = fopen(filePath.c_str(), "w");
    fclose(ofs);
    int state;
    for(int i = 0; i < threadNum; i++){
        string inFilePath("data/tagger_infor/" + fileDirName + "/" + fileDirName + to_string(i));
        string cmd("cat ");
        cmd += inFilePath;
        cmd += " >> ";
        cmd += filePath;
        state = system(cmd.c_str());
        cmd = "rm " + inFilePath;
        state = system(cmd.c_str());
    }
    for(int i = 0; i < threadNum; i++){
        if(maxTag < thTagNum[i])
            maxTag = thTagNum[i];
        delete graphPr[i];
        graphPr[i] = NULL;
    }
    cout << maxTag << endl;
    ofs = fopen(filePath.c_str(), "a");
    fwrite(&maxTag, sizeof(int), 1, ofs);
    fclose(ofs);
    delete graphPr;
    graphPr = NULL;
    delete thTagNum;
    thTagNum = NULL;
}


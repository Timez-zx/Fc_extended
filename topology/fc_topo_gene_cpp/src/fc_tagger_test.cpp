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
                linkLayerMap[src*switches+dst] = (layer_num-i-1)*layer_num+layer_num-i-2;
                linkLayerMap[dst*switches+src] = (layer_num-i-2)*layer_num+layer_num-i-1;
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


int FcTaggerTest::SearchKsp(int srcIn, int dstIn, int pathNum, int vcNum, int thLabel, int *pathInfor){
    KShortestPath ksp(graphPr[thLabel]);
    double cost = ksp.Init(srcIn, dstIn);
    int pathCount = 0, pathLen = 0;
    int tempDst, tempSrc, tempLen, pathLenU;
    int pathU[1000], layerPass[1000];
    int srcInter, dstInter, srcLayer, dstLayer, lastPass, vcUsed, pastLayer;
    while(cost < 10000 & pathCount < pathNum){
        if(cost + 1 > thTagNum[thLabel])
            thTagNum[thLabel] = cost+1;
        pathLenU = cost+1;
        lastPass = -1;
        vcUsed = 1;
        pastLayer = 0;
        vector<Link*> pathTemp = ksp.GetPath();
        tempLen = pathTemp.size();
        for(int i = 0; i < pathLenU-1; i++){
            pathU[i] = pathTemp[i]->source_id;
        }
        pathU[tempLen] = dstIn;
        for(int i = 0; i < pathLenU-1; i++){
            srcInter = pathU[i];
            dstInter = pathU[i+1];
            srcLayer = linkLayerMap[srcInter*switches+dstInter]/layer_num;
            dstLayer = linkLayerMap[srcInter*switches+dstInter]%layer_num;
            layerPass[2*i] = srcLayer;
            layerPass[2*i+1] = dstLayer;
        }
        vector<int> extractLayerPass;
        for(int i = 0; i < 2*(pathLenU-1); i++){
            if(layerPass[i] != lastPass)
                extractLayerPass.push_back(layerPass[i]);
            lastPass = layerPass[i];
        }
        for(int i = 0; i < extractLayerPass.size()-1; i++){
            srcLayer = extractLayerPass[i];
            if(srcLayer < pastLayer && srcLayer < extractLayerPass[i+1]){
                vcUsed++;
            }
            pastLayer = srcLayer;
        }  
        if(vcUsed > vcNum){
            cost = ksp.FindNextPath();
            continue;
        }
        else{
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
    }
    return pathLen;
}


void FcTaggerTest::threadKsp(vector<int*> routePairs, int thLabel, int pathNum, int vcNum, bool ifReport, int reportInter, bool ifStore, string storeFile){
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
        dataNum  = SearchKsp(routePairs[i][0], routePairs[i][1], pathNum, vcNum, thLabel, tempInfor);
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


void FcTaggerTest::mthreadKsp(int threadNum, int pathNum, int vcNum, bool ifReport, int reportInter, bool ifStore){
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
        th[i] = thread(&FcTaggerTest::threadKsp, this, threadPairs[i], i, pathNum, vcNum, ifReport, reportInter, ifStore, fileDirName);
    }
    for(int i = 0; i < threadNum; i++)
        th[i].join();

    string filePath("data/tagger_infor/" + fileDirName + "/" + fileDirName + "_ksp" + to_string(pathNum) + "_vc" + to_string(vcNum));
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
    int batchSize = 100000;
    FILE* ifs = fopen(filePath.c_str(), "r");
    fseek(ifs, 0, SEEK_END);
    long long int lenSize = ftell(ifs);
    long long int remainSize = lenSize/4;
    int *allData = new int[batchSize];
    int readSize;
    rewind(ifs);
    set<vector<int> > dataSet;
    while(remainSize > 0){
        readSize = batchSize >= remainSize ? remainSize:batchSize;
        state = fread(allData, sizeof(int), readSize, ifs);
        for(int i = 0; i < readSize/2; i++){
            vector<int> tempV(2);
            tempV[0] = allData[2*i];
            tempV[1] = allData[2*i+1];
            dataSet.insert(tempV);
        }
        remainSize -= readSize;
    }
    fclose(ifs);
    vector<vector<int> > finalV;
    vector<int> finalData;
    finalV.assign(dataSet.begin(), dataSet.end());
    for(int i = 0; i < finalV.size(); i++){
        finalData.push_back(finalV[i][0]);
        finalData.push_back(finalV[i][1]);
    }
    ofs = fopen(filePath.c_str(), "w");
    fwrite(&finalData[0], sizeof(int), finalData.size(), ofs);
    cout << maxTag << endl;
    int sw = switches;
    int totalPort = ports-hosts+1;
    fwrite(&maxTag, sizeof(int), 1, ofs);
    fwrite(&sw, sizeof(int), 1, ofs);
    fwrite(&totalPort, sizeof(int), 1, ofs);
    fclose(ofs);
    delete graphPr;
    graphPr = NULL;
    delete thTagNum;
    thTagNum = NULL;
    delete allData;
    allData = NULL;
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
    int batchSize = 100000;
    FILE* ifs = fopen(filePath.c_str(), "r");
    fseek(ifs, 0, SEEK_END);
    long long int lenSize = ftell(ifs);
    long long int remainSize = lenSize/4;
    int *allData = new int[batchSize];
    int readSize;
    rewind(ifs);
    set<vector<int> > dataSet;
    while(remainSize > 0){
        readSize = batchSize >= remainSize ? remainSize:batchSize;
        state = fread(allData, sizeof(int), readSize, ifs);
        for(int i = 0; i < readSize/2; i++){
            vector<int> tempV(2);
            tempV[0] = allData[2*i];
            tempV[1] = allData[2*i+1];
            dataSet.insert(tempV);
        }
        remainSize -= readSize;
    }
    fclose(ifs);
    ofs = fopen(filePath.c_str(), "w");
    vector<vector<int> > finalV;
    vector<int> finalData;
    finalV.assign(dataSet.begin(), dataSet.end());
    for(int i = 0; i < finalV.size(); i++){
        finalData.push_back(finalV[i][0]);
        finalData.push_back(finalV[i][1]);
    }
    fwrite(&finalData[0], sizeof(int), finalData.size(), ofs);
    cout << maxTag << endl;
    int sw = switches;
    int totalPort = ports-hosts+1;
    fwrite(&maxTag, sizeof(int), 1, ofs);
    fwrite(&sw, sizeof(int), 1, ofs);
    fwrite(&totalPort, sizeof(int), 1, ofs);
    fclose(ofs);
    delete graphPr;
    graphPr = NULL;
    delete thTagNum;
    thTagNum = NULL;
    delete allData;
    allData = NULL;
}


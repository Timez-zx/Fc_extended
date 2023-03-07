#include "fc_extended.h"





FcExtended::~FcExtended(){
    if(graphPr != NULL){
        delete[] graphPr;
        graphPr = NULL;
    }
    if (bitMap)
    {
        for(int i = 0; i < switches; i++){
            if(bitMap[i]){
                delete[] bitMap[i];
                bitMap[i] = NULL;
            }
        }   
        delete[] bitMap;
        bitMap = NULL;
    }    
}


FcExtended::FcExtended(const int switchIn, const int layerIn, const int totalPortIn, const std::vector<int>& layerDegreeIn) {
    if(accumulate(layerDegreeIn.begin(), layerDegreeIn.end(),0) != totalPortIn){
        show("The number of ports is diff with the vec!");
        exit(1);
    }
    switches = switchIn;
    layerNum = layerIn;
    totalUpPort = totalPortIn;
    layerDegrees.assign(layerDegreeIn.begin(), layerDegreeIn.end());
}


void FcExtended::GeneTopo(){
    linkInfor.reserve(totalUpPort*switches);
    std::vector<std::vector<int> > possibleConnect(switches);
    std::vector<int> tempVec(switches);
    InitVectorInt(tempVec);
    for(int i = 0; i < switches; i++){
        possibleConnect[i].assign(tempVec.begin(), tempVec.end());
        RemoveVecEle(possibleConnect[i], i);
    }
    bitMap = new int*[switches];
    for(int i = 0; i < switches; i++){
        bitMap[i] = new int[switches];
        memset(bitMap[i], 0, sizeof(int)*switches);
    }
    srand(randomSeed);
    GeneLink(possibleConnect);
}


void FcExtended::GeneLink(std::vector<std::vector<int> > &possibleConnect){
    int lastLayerRemainDegree = 0;
    int layerDegree, twoCount, zeroCount, deadCycleBreak, randIndex, minimum, tempCount;
    int src, dst;
    int *swDegrees = new int[switches];
    int *swDegreeLabel = new int[switches];
    bool **vertexConnect = new bool*[switches];
    std::vector<int> srcChoose;
    for(int i = 0; i < switches; i++)
        vertexConnect[i] = new bool[switches]();
    for(int i = layerNum - 1; i > 0; i--){
        layerDegree = layerDegrees[i] - lastLayerRemainDegree;
        for(int j = 0; j < layerDegree; j++){
            zeroCount = switches;
            twoCount = 0;
            memset(swDegrees, 0, sizeof(int)*switches);
            memset(swDegreeLabel, 0, sizeof(int)*switches);
            srcChoose.clear();
            srcChoose.resize(switches);
            for(int k = 0; k < switches; k++)
                srcChoose[k] = k;
            src = 0;
            while(twoCount < switches){
                deadCycleBreak = 0;
                randIndex = rand()%possibleConnect[src].size();
                dst = possibleConnect[src][randIndex];
                swDegrees[src]++;
                swDegrees[dst]++;
                if(swDegrees[src] == 1)
                    zeroCount--;
                if(swDegrees[dst] == 1)
                    zeroCount--;
                if(swDegrees[src] == 2)
                    twoCount++;
                if(swDegrees[dst] == 2)
                    twoCount++;
                while(vertexConnect[src][dst] || (twoCount+zeroCount==switches && zeroCount < 3 && srcChoose.size() != 1) || swDegreeLabel[dst]==1){
                    if(swDegrees[dst] == 2)
                        twoCount--;
                    if(swDegrees[dst] == 1)
                        zeroCount++;
                    swDegrees[dst]--;
                    randIndex = rand()%possibleConnect[src].size();
                    dst = possibleConnect[src][randIndex];
                    swDegrees[dst]++;
                    if(deadCycleBreak > 1e4){
                        std::cerr << "Can't construct, please change the rand seed or use other mode!" << std::endl;
                        exit(1);
                    }
                    deadCycleBreak++;
                    if(swDegrees[dst] == 1)
                        zeroCount--;
                    if(swDegrees[dst] == 2)
                        twoCount++;
                }
                vertexConnect[src][dst] = true;
                vertexConnect[dst][src] = true;
                swDegreeLabel[dst] = 1;
                linkInfor.push_back(SwLink(SwNode(src, i), SwNode(dst, i-1)));
                bitMap[src][dst] = 1;
                bitMap[dst][src] = 1;
                RemoveVecEle(possibleConnect[src], dst);
                RemoveVecEle(possibleConnect[dst], src);
                RemoveVecEle(srcChoose, src);
                if(fastTopoBuild)
                    src++;
                else{
                    minimum = switches+1;
                    for(int k = 0; k < srcChoose.size(); k++){
                        tempCount = 0;
                        for(int m = 0; m < switches; m++)
                            tempCount += swDegreeLabel[m] | vertexConnect[srcChoose[k]][m];
                        tempCount = switches - tempCount;
                        if(tempCount <= minimum){
                            src = srcChoose[k];
                            minimum = tempCount;
                        }
                    }
                }
            }
        }
        lastLayerRemainDegree = layerDegree;
    }
    delete [] swDegrees;
    delete [] swDegreeLabel;
    for(int i = 0; i <  switches; i++){
        delete[] vertexConnect[i];
    }
    delete[] vertexConnect;
    show(linkInfor.size());
    show("Topo constructed!");
}


void FcExtended::DFS(const std::vector<int>& heads, const std::vector<Edge>& edges, int start, int end){
    if(returnFlag == 1)
        return;
    visitedGlobal[start] = 1;
    stackGlobal.push_back(start);
    int nearVertex;
    if(start == end){
        for(int i = 0; i < stackGlobal.size()/2; i++){
        // for(int i = 0; i < 2; i++){
            if(GetSwLabel(stackGlobal[i]) != GetSwLabel(stackGlobal[stackGlobal.size()-i-1])){
                returnFlag = 1;
                break;
            }
        }
        stackGlobal.pop_back();
        visitedGlobal[start] = 0;
        return;
    }
    for(int i = heads[start]; i != -1; i = edges[i].nextEdgeIdex){
        nearVertex = edges[i].toNode;
        if(!visitedGlobal[nearVertex]){
            DFS(heads, edges, nearVertex, end);
        }
    }
    stackGlobal.pop_back();
    visitedGlobal[start] = 0;

}


inline int FcExtended::GetVertexLabel(int swLabel, int layer, int ifDown){
    return ifDown*switches*layerNum+layer*switches+swLabel;
}

inline int FcExtended::GetSwLabel(int vertexLabel){
    return (vertexLabel%(switches*layerNum))%switches;
}


void FcExtended::GetCycleEdge(int layer){
    int totalNode, maxEdgeNum, edgeCount = 0, initDegree = 0, lastIndex, realLayer, cycleVer;
    int totalEdgeAdd = 0;
    totalNode = switches*layerNum*2;
    maxEdgeNum = switches*layerNum*totalUpPort*10;
    std::vector<int> heads(totalNode, -1);
    std::vector<Edge> edges(maxEdgeNum);
    std::vector<int> linkIndex;
    std::vector<int> globalVertex(switches);
    std::vector<int> partialVertex;
    InitVectorInt(globalVertex);
    linkIndex.push_back(0);
    lastIndex = 0;
    visitedGlobal.resize(totalNode);
    swTovirLayer.resize(switches);
    for(int i = layerNum-1; i > 0; i--){
        linkIndex.push_back((layerDegrees[i]-initDegree)*switches+lastIndex);
        lastIndex += (layerDegrees[i]-initDegree)*switches;
        initDegree = layerDegrees[i]-initDegree;
    }
    for(int i = 0; i < switches; i++){
        AddEdges(heads, edges, GetVertexLabel(i,layerNum-1,0), GetVertexLabel(i,layerNum-1,1), edgeCount);
        swTovirLayer[i] = layerNum-1;
    }
    for(int i = 0; i < layerNum-1; i++){
        realLayer = layerNum-i-2;
        for(int j = 0; j < switches; j++){
            AddEdges(heads, edges, GetVertexLabel(j,realLayer,0), GetVertexLabel(j,realLayer+1,0), edgeCount);
            AddEdges(heads, edges, GetVertexLabel(j,realLayer+1,1), GetVertexLabel(j,realLayer,1), edgeCount);
        }
        for(int j = linkIndex[i]; j < linkIndex[i+1]; j++){
            AddEdges(heads, edges, GetVertexLabel(linkInfor[j].srcNode.swLabel,realLayer+1,1), GetVertexLabel(linkInfor[j].dstNode.swLabel,realLayer,1), edgeCount);
            AddEdges(heads, edges, GetVertexLabel(linkInfor[j].dstNode.swLabel,realLayer,0), GetVertexLabel(linkInfor[j].srcNode.swLabel,realLayer+1,0), edgeCount);
        }
        // if(realLayer <= layer){
        //     partialVertex.assign(globalVertex.begin(), globalVertex.end());
        //     while(partialVertex.size() > 0){
        //         memset(&visitedGlobal[0], 0, sizeof(int)*totalNode);
        //         stackGlobal.clear();
        //         maxNodePass = i+2;
        //         returnFlag = 0;
        //         cycleVer = partialVertex[rand()%partialVertex.size()];
        //         DFS(heads, edges, GetVertexLabel(cycleVer,realLayer,0), GetVertexLabel(cycleVer,realLayer,1));
        //         if(returnFlag){
        //             RemoveVecEle(globalVertex, cycleVer);
        //             RemoveVecEle(partialVertex, cycleVer);
        //         }
        //         else{
        //             totalEdgeAdd++;
        //             swTovirLayer[cycleVer] = realLayer;
        //             // std::cout << GetVertexLabel(cycleVer,realLayer,1) << " " << GetVertexLabel(cycleVer,realLayer,0) << std::endl;
        //             AddEdges(heads, edges, GetVertexLabel(cycleVer,realLayer,1), GetVertexLabel(cycleVer,realLayer,0), edgeCount);
        //             RemoveVecEle(partialVertex, cycleVer);
        //         }
        //     }
        // }
    }
    while(globalVertex.size() > 0){
        cycleVer = globalVertex[rand()%globalVertex.size()];
        for(int i = 0; i < layerNum-1; i++){
            realLayer = layerNum-i-2;
            memset(&visitedGlobal[0], 0, sizeof(int)*totalNode);
            stackGlobal.clear();
            returnFlag = 0;
            DFS(heads, edges, GetVertexLabel(cycleVer,realLayer,0), GetVertexLabel(cycleVer,realLayer,1));
            if(returnFlag){
                break;
            }
            else{
                totalEdgeAdd++;
                swTovirLayer[cycleVer] = realLayer;
                if(i > 0)
                    DeleLastEdges(heads, edges, edgeCount);
                AddEdges(heads, edges, GetVertexLabel(cycleVer,realLayer,1), GetVertexLabel(cycleVer,realLayer,0), edgeCount);
            }
        }
        RemoveVecEle(globalVertex, cycleVer);
    }
    PrintVectorInt(swTovirLayer);
    show(totalEdgeAdd);
}


std::string FcExtended::GenePath(const std::string &path){
    if(access(path.c_str(), 0)){
        std::string cmd("mkdir ");
        cmd += path;
        int temp = system(cmd.c_str());
    }
    std::string fileDirName("");
    fileDirName += "sw";
    fileDirName += std::to_string(switches);
    fileDirName += "_vir";
    for(int i = 0; i < layerNum; i++)
        fileDirName += std::to_string(layerDegrees[i]);
    fileDirName += "_rand";
    fileDirName += std::to_string(randomSeed);
    if(access((path + fileDirName).c_str(), 0)){
        std::string cmd("mkdir ");
        cmd += path;
        cmd += fileDirName;
        int temp = system(cmd.c_str());
    }
    return fileDirName;
}


void FcExtended::SaveTopoInfor(){
    if(linkInfor.size() == 0){
        std::cerr << "Please generate graph topology!" << std::endl;
        exit(1);
    }
    std::string fileDirPath = GenePath("data/topo_infor/");
    std::ofstream ofs("data/topo_infor/" + fileDirPath + "/" + fileDirPath+".txt");
    int degree;
    int basic_index = 0, count = 0;
    int src, dst, srcLayer, dstLayer;
    ofs << "LinkID,SourceID,DestinationID,PeerID,Cost,Bandwidth,Delay,SRLGNum,SRLGs" << std::endl;
    for(int i = 0; i < linkInfor.size(); i++){
        src = linkInfor[i].srcNode.swLabel;
        srcLayer = linkInfor[i].srcNode.layerLabel;
        dst = linkInfor[i].dstNode.swLabel;
        dstLayer = linkInfor[i].dstNode.layerLabel;
        swPairToLayerPair[GetHash(src, dst, switches)] = GetHash(srcLayer, dstLayer, layerNum);
        swPairToLayerPair[GetHash(dst, src, switches)] = GetHash(dstLayer, srcLayer, layerNum);
        ofs << count++ << "," << src << "," << dst << "," << 0 << "," << 1 << ",0,0,0,0"<< std::endl;
        ofs << count++ << "," << dst << "," << src << "," << 0 << "," << 1 << ",0,0,0,0"<< std::endl;
    }
    ofs.close();
    topoPath = "data/topo_infor/" + fileDirPath + "/" + fileDirPath+".txt";
}


std::string FcExtended::GenePathKsp(const std::string& path, int pathNum, int vcNum){
    if(access(path.c_str(), 0)){
        std::string cmd("mkdir ");
        cmd += path;
        int temp = system(cmd.c_str());
    }
    std::string fileDirPath("");
    fileDirPath += "sw";
    fileDirPath += std::to_string(switches);
    fileDirPath += "_vir";
    for(int i = 0; i < layerNum; i++)
        fileDirPath += std::to_string(layerDegrees[i]);
    fileDirPath += "_rand";
    fileDirPath += std::to_string(randomSeed);
    fileDirPath += "_";
    fileDirPath += std::to_string(pathNum);
    fileDirPath += "_";
    fileDirPath += std::to_string(vcNum);
    if(access((path + fileDirPath).c_str(), 0)){
        std::string cmd("mkdir ");
        cmd += path;
        cmd += fileDirPath;
        int temp = system(cmd.c_str());
    }
    return fileDirPath;
}


void FcExtended::MthreadKsp(int threadNum, int pathNum, int vcNum, bool ifReport, int reportInter){
    if(linkInfor.size() == 0){
        std::cerr << "Please generate graph topology!" << std::endl;
        exit(1);
    }
    int totalPairs = switches*(switches-1)/2;
    int average = ceil(totalPairs/float(threadNum));
    int count = 0;
    int alloIndex = 0;
    std::vector<std::vector<Pair> > threadPairs(threadNum);
    for(int i = 0; i < switches; i++){
        for (int j = i+1; j < switches; j++)
        {
            threadPairs[alloIndex].push_back(Pair(i, j));
            if(count < average - 1)
                count++;
            else{
                count = 0;
                alloIndex++;
            }
        } 
    }

    graphPr = new Graph*[threadNum];
    for(int i = 0; i < threadNum; i++)
        graphPr[i] = new Graph(topoPath);
    
    std::string fileDirName("");
    fileDirName += GenePathKsp("data/all_graph_route_ksp/", pathNum, vcNum);

    std::thread* th = new std::thread[threadNum];
    for(int i = 0; i < threadNum; i++)
        th[i] = std::thread(&FcExtended::ThreadKsp, this, threadPairs[i], i, pathNum, vcNum, ifReport, reportInter, fileDirName);
    for(int i = 0; i < threadNum; i++)
        th[i].join();

    std::string filePath("data/all_graph_route_ksp/" + fileDirName + "/" + fileDirName);
    std::string lenPath(filePath + "_num"); 
    int state;
    FILE* ofs = fopen(filePath.c_str(), "w");
    FILE* ofsLen = fopen(lenPath.c_str(), "w");
    for(int i = 0; i < threadNum; i++){
        std::string inFilePath("data/all_graph_route_ksp/" + fileDirName + "/" + fileDirName + std::to_string(i));
        std::string inLenPath(inFilePath + "_num");
        std::string cmd1("cat ");
        std::string cmd2("cat ");
        cmd1 += inFilePath;
        cmd1 += " >> ";
        cmd1 += filePath;
        cmd2 += inLenPath;
        cmd2 += " >> ";
        cmd2 += lenPath;
        state = system(cmd1.c_str());
        state = system(cmd2.c_str());
        cmd1 = "rm " + inFilePath;
        cmd2 = "rm " + inLenPath;
        state = system(cmd1.c_str());
        state = system(cmd2.c_str());
    }
    fclose(ofs);
    fclose(ofsLen);
    for(int i = 0; i < threadNum; i++){
        delete graphPr[i];
        graphPr[i] = NULL;
    }
    delete graphPr;
    graphPr = NULL;
}


void FcExtended::ThreadKsp(const std::vector<Pair> &routePairs, int threadLabel, int pathNum, int vcNum, bool ifReport, int reportInter, std::string storeFile){
    int count = 0, storeCount = 0;
    uint16_t dataNum;
    FILE* ofs;
    FILE* ofsLen;
    uint16_t* tempData;
    uint16_t* tempInfor = new uint16_t[switches*1000];
    uint16_t** storePairsInfo = new uint16_t*[reportInter];
    int pathFind;
    std::vector<uint16_t> storeInfoLen;

    std::string filePath("data/all_graph_route_ksp/" + storeFile + "/" + storeFile + std::to_string(threadLabel));
    std::string lenPath(filePath + "_num");
    ofs = fopen(filePath.c_str(), "w");
    ofsLen = fopen(lenPath.c_str(), "w");

    for(int i = 0; i < routePairs.size(); i++){
        dataNum  = SearchKsp(routePairs[i].src, routePairs[i].dst, pathNum, vcNum, tempInfor, threadLabel, pathFind);
        if(ifReport){
            count++;
            if(count % reportInter == 0)
                std::cout << "The thread " << threadLabel << " " <<count/double(routePairs.size()) << std::endl;
        }
        tempData = new uint16_t[dataNum];
        memcpy(tempData, tempInfor, sizeof(uint16_t)*dataNum);
        storePairsInfo[storeCount] = tempData;
        storeInfoLen.push_back(pathFind);
        storeInfoLen.push_back(dataNum);
        storeCount++;
        if(storeCount == reportInter) {
            fwrite(&storeInfoLen[0], sizeof(uint16_t), storeCount*2, ofsLen);
            for(int j = 0; j < reportInter; j++){
                fwrite(storePairsInfo[j], sizeof(uint16_t), storeInfoLen[2*j+1], ofs);
                delete[] storePairsInfo[j];
                storePairsInfo[j] = NULL;
            }
            fflush(ofs);
            storeInfoLen.clear();
            storeCount = 0;
        }
    }
    fwrite(&storeInfoLen[0], sizeof(uint16_t), storeCount*2, ofsLen);
    for(int j = 0; j < storeCount; j++){
        fwrite(storePairsInfo[j], sizeof(uint16_t), storeInfoLen[2*j+1], ofs);
        delete[] storePairsInfo[j];
    }
    fclose(ofs);
    fclose(ofsLen);
    delete[] tempInfor;
    tempInfor = NULL;
    delete[] storePairsInfo;
    storePairsInfo = NULL;
}


uint16_t FcExtended::SearchKsp(int src, int dst, int pathNum, int vcNum, uint16_t *pathInfor, int threadLabel, int &pathFind){
    KShortestPath ksp(graphPr[threadLabel]);
    double cost = ksp.Init(src, dst);
    int pathCount = 0, pathLen, vcUsed, pastLayer, lastPass, layerHashValue;
    int srcInter, dstInter, srcLayer, dstLayer;
    int path[1000], layerPass[1000], swPass[1000];
    uint16_t dataNum=0, realPath[1000];
    std::vector<int> layerInfor(2);
    std::vector<Link*> shortestPath;
    std::vector<int> extractLayerPass;
    std::vector<int> extractSwPass;

    while(cost < 10000 & pathCount < pathNum){
        vcUsed = 1;
        pastLayer = 0;
        lastPass = -1;
        shortestPath.clear();
        extractLayerPass.clear();
        extractSwPass.clear();
        shortestPath = ksp.GetPath();
        pathLen = cost+1;
        for(int i = 0; i < shortestPath.size(); i++)
            path[i] = shortestPath[i]->source_id;
        path[shortestPath.size()] = dst;
        for(int i = 0; i < pathLen-1; i++){
            srcInter = path[i];
            dstInter = path[i+1];
            layerHashValue = swPairToLayerPair[GetHash(srcInter, dstInter, switches)];
            GetValue(layerHashValue, layerNum, layerInfor);
            srcLayer = layerInfor[0];
            dstLayer = layerInfor[1];
            layerPass[2*i] = srcLayer;
            layerPass[2*i+1] = dstLayer;
            swPass[2*i] = path[i];
            swPass[2*i+1] = path[i+1];
        }
        for(int i = 0; i < 2*(pathLen-1); i++){
            if(layerPass[i] != lastPass){
                extractLayerPass.push_back(layerPass[i]);
                extractSwPass.push_back(swPass[i]);
            }
            lastPass = layerPass[i];
        }
        for(int i = 0; i < extractLayerPass.size()-1; i++){
            srcLayer = extractLayerPass[i];
            if(srcLayer < pastLayer && srcLayer < extractLayerPass[i+1]){
                if(swTovirLayer[extractSwPass[i]] > srcLayer){
                    vcUsed++;
                }
                // else{
                //     PrintArrayInt(layerPass, 2*(pathLen-1));
                //     // std::cout << swTovirLayer[extractSwPass[i]] << " " << srcLayer << std::endl;
                // }
            }
            pastLayer = srcLayer;
        } 
        cost = ksp.FindNextPath();
        if(vcUsed > vcNum)
            continue;
        else{
            // PrintArrayInt(path, pathLen);
            // PrintArrayInt(layerPass, 2*(pathLen-1));
            pathCount++;
            for(int i = 0; i < pathLen-1; i++){
                if(layerPass[2*i] < layerPass[2*i+1]){
                    realPath[2*i] = layerPass[2*i]*switches + path[i];
                    realPath[2*i+1] = layerPass[2*i+1]*switches + path[i+1];
                }
                else if(layerPass[2*i] > layerPass[2*i+1]){
                    realPath[2*i] = layerPass[2*i]*switches + path[i] + (layerNum-layerPass[2*i]-1)*2*switches;
                    realPath[2*i+1] = layerPass[2*i+1]*switches + path[i+1] + (layerNum-layerPass[2*i+1]-1)*2*switches;
                }
                pathInfor[dataNum + 2*i] = realPath[2*i];
                pathInfor[dataNum + 2*i + 1] = realPath[2*i+1];
            }
            dataNum += 2*(pathLen-1);
        }
    }
    pathFind = pathCount;
    return dataNum;
}


void FcExtended::GeneUniformRandom(float **flowMatrix, int seed, int hosts){
    int combination = switches/8, sum = 0, storeSame, storeLen, randPick;
    int randRate[combination], permu[switches];
    float rate[combination];
    srand(seed);
    for(int i = 0; i < combination; i++){
        randRate[i] = rand()%10000;
        sum += randRate[i];
    }
    for(int i = 0; i < combination; i++)
        rate[i] = randRate[i]/float(sum);
    for(int i = 0; i < switches; i++)
        for(int j = 0; j < switches; j++)
            flowMatrix[i][j] = 0;
    for(int k = 0; k < combination; k++){
        for(int i = 0; i < switches; i++)
            permu[i] = i;
        std::shuffle(permu, permu+switches, std::default_random_engine((k+1)*seed));
        storeLen = 0;
        for(int i = 0; i < switches; i++){
            if(permu[i] == i){
                if(storeLen == 0){
                    storeSame = i;
                    storeLen++;
                }
                else{
                    permu[i] = storeSame;
                    permu[storeSame] = i;
                    storeLen--;
                }
            }
        }
        if(storeLen == 1){
            randPick = rand()%switches;
            while(randPick == storeSame)
                randPick = rand()%switches;
            permu[randPick] = storeSame;
            permu[storeSame] = randPick;
        }
        for(int i = 0; i < switches; i++)
            flowMatrix[i][permu[i]] += rate[k]*hosts;
    }
}


void FcExtended::GeneWorseCase(float **flowMatrix, int hosts){
    if(access("data/worst_flow_infor", 0)){
        std::string cmd("mkdir ");
        cmd += "data/worst_flow_infor";
        int temp = system(cmd.c_str());
    }
    std::string storeFile("temp_infor");
    std::string filePath("data/worst_flow_infor/" + storeFile);
    FILE* ofs = fopen(filePath.c_str(), "w");
    fwrite(&switches, sizeof(int), 1, ofs);
    fwrite(&hosts, sizeof(int), 1, ofs);
    for(int i = 0; i < switches; i++)
        fwrite(bitMap[i], sizeof(int), switches, ofs);
    fclose(ofs);
    std::string cmd("python3 src/worst_case.py");
    int state = system(cmd.c_str());

    std::string readFile("temp_infor_flow");
    std::string readPath("data/worst_flow_infor/" + readFile);
    std::ifstream ifs(readPath.c_str(), std::ios::in);
    for(int i = 0; i < switches; i++){
        for(int j = 0; j < switches; j++)
            ifs >> flowMatrix[i][j];
    }
    ifs.close();
}


double FcExtended::throughputTest(const std::string& type, int seed, int pathNum, int vcNum, int hosts){
    std::string fileDirName("");
    fileDirName += "sw";
    fileDirName += std::to_string(switches);
    fileDirName += "_vir";
    for(int i = 0; i < layerNum; i++)
        fileDirName += std::to_string(layerDegrees[i]);
    fileDirName += "_rand";
    fileDirName += std::to_string(randomSeed);
    fileDirName += "_";
    fileDirName += std::to_string(pathNum);
    fileDirName += "_";
    fileDirName += std::to_string(vcNum);
    if(access(("data/all_graph_route_ksp/" + fileDirName).c_str(), 0)){
        std::cerr << "No route infor!" << std::endl;
        exit(1);
    }

    std::string filePath("data/all_graph_route_ksp/" + fileDirName + "/" + fileDirName);
    std::string lenPath(filePath + "_num"); 
    int state;
    FILE* ofs = fopen(filePath.c_str(), "r");
    FILE* ofsLen = fopen(lenPath.c_str(), "r");
    fseek(ofsLen, 0, SEEK_END);
    int lenSize = ftell(ofsLen);
    rewind(ofsLen);
    fseek(ofs, 0, SEEK_END);
    int64_t fileSize = ftell(ofs);
    rewind(ofs);
    uint16_t *pairLen = new uint16_t[lenSize/2];
    uint16_t *pairInfor = new uint16_t[fileSize/2];
    state = fread(pairLen, sizeof(uint16_t), lenSize/2, ofsLen);
    state = fread(pairInfor, sizeof(uint16_t), fileSize/2, ofs);

    std::unordered_map<int, std::vector<int> > pathToLink;
    int nodeLen, basicLen = 0, count, pathCount;
    int node1, node2, src = 0, dst = 1;
    int basicCount = 1, mapCount;
    int sw1, sw2, layer1, layer2, newNode1, newNode2;

    float *flowMatrix[switches];
    for(int i = 0; i < switches; i++){
        flowMatrix[i] = new float[switches];
        memset(flowMatrix[i], 0, sizeof(float)*switches);
    }
    if(type == "aa"){
        float pairFlow = hosts/float(switches-1);
        for(int i = 0; i < switches; i++)
            for(int j = 0; j < switches; j++)
                if(i != j)
                    flowMatrix[i][j] = pairFlow;
    }
    else if(type == "ur")
        GeneUniformRandom(flowMatrix, seed, hosts);
    else if(type == "wr")
        GeneWorseCase(flowMatrix, hosts);

    int maxThrough = 10, pathSum = 0;
    int *pathNumber[switches];
    GRBEnv *env = new GRBEnv();
    GRBModel model = GRBModel(*env);
    GRBVar **flowVar;
    flowVar = new GRBVar*[switches*switches];
    for(int i = 0; i < switches; i++)
        pathNumber[i] = new int[switches];
    GRBVar throughput = model.addVar(0, maxThrough, 0, GRB_CONTINUOUS, "throughput");

    for(int i = 0; i < switches*(switches-1)/2; i++){
        nodeLen = pairLen[2*i+1];
        count = 0;
        pathCount = 0;
        pathSum += pairLen[2*i];
        if(flowMatrix[src][dst] > 0){
            flowVar[src*switches+dst] = model.addVars(pairLen[2*i], GRB_CONTINUOUS);
            pathNumber[src][dst] = pairLen[2*i];
            for(int j = 0; j < pairLen[2*i]; j++){
                flowVar[src*switches+dst][j].set(GRB_DoubleAttr_LB, 0.0);
                flowVar[src*switches+dst][j].set(GRB_DoubleAttr_UB, 100);
            }
        }
        if(flowMatrix[dst][src] > 0){
            flowVar[dst*switches+src] = model.addVars(pairLen[2*i], GRB_CONTINUOUS);
            pathNumber[dst][src] = pairLen[2*i];
            for(int j = 0; j < pairLen[2*i]; j++){
                flowVar[dst*switches+src][j].set(GRB_DoubleAttr_LB, 0.0);
                flowVar[dst*switches+src][j].set(GRB_DoubleAttr_UB, 100);
            }
        }
        while(count < nodeLen - 1){
            node1 = pairInfor[basicLen+count];
            node2 = pairInfor[basicLen+count+1];
            sw1 = node1%switches;
            sw2 = node2%switches;
            layer1 = node1/switches;
            layer2 = node2/switches;
            if(sw1 != sw2){
                if(flowMatrix[src][dst] > 0){
                    mapCount = node1*(2*layerNum-1)*switches+node2;
                    if(pathToLink.find(mapCount) == pathToLink.end()){
                        std::vector<int> temp = {src*switches+dst, pathCount};
                        pathToLink[mapCount] = temp;
                    }
                    else{
                        pathToLink[mapCount].push_back(src*switches+dst);
                        pathToLink[mapCount].push_back(pathCount);
                    }
                }
                if(flowMatrix[dst][src] > 0){
                    newNode1 = node1 + (layerNum-1-layer1)*2*switches;
                    newNode2 = node2 + (layerNum-1-layer2)*2*switches;
                    mapCount = newNode2*(2*layerNum-1)*switches+newNode1;
                    if(pathToLink.find(mapCount) == pathToLink.end()){
                        std::vector<int> temp = {dst*switches+src, pathCount};
                        pathToLink[mapCount] = temp;
                    }
                    else{
                        pathToLink[mapCount].push_back(dst*switches+src);
                        pathToLink[mapCount].push_back(pathCount);
                    }
                }
            }
            if(sw2 == dst){
                count++;
                pathCount++;
            }
            count++;
        }
        dst++;
        if(dst == switches){
            src = basicCount;
            dst = ++basicCount;
        }
        basicLen += nodeLen;
    }
    fclose(ofs);
    fclose(ofsLen);
    std::cout << "Average path num: " << float(pathSum)*2/(switches*(switches-1)) << std::endl; 

    for(int i = 0; i < switches; i++){
        for(int j = 0; j < switches; j++){
            if(i != j){
                if(flowMatrix[i][j] > 0){
                    GRBLinExpr constr = 0;
                    for (int k = 0; k < pathNumber[i][j]; k++) 
                        constr += flowVar[i*switches+j][k];
                    constr -= throughput * flowMatrix[i][j];
                    model.addConstr(constr, GRB_EQUAL, 0);
                }
            }
        }
    }  
    for(auto &link : pathToLink){
        GRBLinExpr constr = 0;
        for(int i = 0; i < link.second.size()/2; i++)
            constr += flowVar[link.second[2*i]][link.second[2*i+1]];
        model.addConstr(constr, GRB_LESS_EQUAL, 1);
    }
    std::cout << "All consts added" << std::endl;

    model.setObjective(1 * throughput, GRB_MAXIMIZE);
    model.set(GRB_IntParam_OutputFlag, 0);
    double throughtResult = 0.0;
    try {
        model.optimize();
        if(model.get(GRB_IntAttr_Status) != GRB_OPTIMAL)
            std::cout << "Not optimal " << std::endl;
        else{
            throughtResult = model.get(GRB_DoubleAttr_ObjVal);
            std::cout << "Throughtput: " << throughtResult << std::endl;
        }
    } catch (GRBException e) {
        std::cout << "Error code = " << e.getErrorCode() << std::endl;
        std::cout << e.getMessage() << std::endl;
    }

    for(int i = 0; i < switches; i++)
        delete[] flowMatrix[i];
    delete[] pairLen;
    delete[] pairInfor;
    for(int i = 0; i < switches; i++){
        delete[] pathNumber[i];
    }
    delete[] flowVar;
    return throughtResult;
}




#include "fc_with_flat_edge.h"

FcWithFlatEdge::FcWithFlatEdge(const int switchIn, const int layerIn, const int totalPortIn, const std::vector<int>& upDownIn, const std::vector<int>& flatIn) {
    switches = switchIn;
    layerNum = layerIn;
    totalUpPort = totalPortIn;
    upDownDegree.assign(upDownIn.begin(), upDownIn.end());
    flatEdgeLayerNum.assign(flatIn.begin(), flatIn.end());
}


void FcWithFlatEdge::GeneTopo() {
    int totalUpDownDegree = accumulate(upDownDegree.begin(), upDownDegree.end(),0);
    int maxFlatEdge = (totalUpPort-totalUpDownDegree)*switches/2;
    int flatEdgeNum = accumulate(flatEdgeLayerNum.begin(), flatEdgeLayerNum.end(),0);
    if(flatEdgeNum > maxFlatEdge){
        std::cerr << "Too many flat edges, Please change the number of flat edges, at most " << maxFlatEdge << "!\n";
        exit(1);
    }
    for(auto layerEdgeNum:flatEdgeLayerNum){
        if(layerEdgeNum >= switches){
            std::cerr << "For uniformity, the flat edge number of a layer should be less than switches number: " << switches << "!\n";
            exit(1);
        }
    }
    linkInfor.reserve(totalUpDownDegree*switches/2+flatEdgeNum);

    std::vector<std::vector<int> > possibleConnect(switches);
    std::vector<int> tempVec(switches);
    std::vector<int> srcChoose;
    int lastLayerRemainDegree = 0;
    int layerDegree, twoCount, zeroCount, deadCycleBreak, randIndex, minimum, tempCount;
    int src, dst;
    int *swDegrees = new int[switches];
    int *swDegreeLabel = new int[switches];
    bool **vertexConnect = new bool*[switches];
    for(int i = 0; i < switches; i++)
        tempVec[i] = i;
    for(int i = 0; i < switches; i++){
        possibleConnect[i].assign(tempVec.begin(), tempVec.end());
        RemoveVecEle(possibleConnect[i], i);
    }
    srand(randomSeed);
    GeneUpDownTopo(possibleConnect);
    GeneFlatTopo(possibleConnect);
}



void FcWithFlatEdge::GeneUpDownTopo(std::vector<std::vector<int> > &possibleConnect){
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
        layerDegree = upDownDegree[i] - lastLayerRemainDegree;
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
                    if(deadCycleBreak > 1e6){
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
                linkInfor.push_back(SwLink(SwNode(dst, i-1), SwNode(src, i)));
                // std::cout << src << " " << dst << "\n";
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
        // std::cout <<"\n";
    }
    delete [] swDegrees;
    delete [] swDegreeLabel;
    for(int i = 0; i <  switches; i++){
        delete[] vertexConnect[i];
    }
    delete[] vertexConnect;
    std::cout <<"Up down edge constructed!\n";
}


void FcWithFlatEdge::GeneFlatTopo(std::vector<std::vector<int> > &possibleConnect) {
    int totalUpDownDegree = accumulate(upDownDegree.begin(), upDownDegree.end(),0);
    int flatDegree = totalUpPort - totalUpDownDegree;
    int maxOutDegree = flatDegree/2,  maxInDegree = flatDegree/2;
    int src, dst, srcIndex, dstIndex, remainFlatEdge, layerCount = 0, geneEdgeNum, deadCycleBreak=0;
    std::vector<int> randSwVec(switches), outRemainDegrees(switches), inRemainDegrees(switches);
    std::vector<int> tempSrcVec, tempDstVec;
    for(int i = 0; i < switches; i++){
        outRemainDegrees[i] = maxOutDegree;
        inRemainDegrees[i] = maxInDegree;
        randSwVec[i] = i;
    }

    while(layerCount < layerNum){
        remainFlatEdge = flatEdgeLayerNum[layerCount];
        shuffle(randSwVec.begin(), randSwVec.end(), std::default_random_engine(randomSeed));
        srcIndex = 0;
        dstIndex = 1;
        geneEdgeNum = 0;
        tempSrcVec.clear();
        tempDstVec.clear();
        while(remainFlatEdge > 0){
            src = randSwVec[srcIndex];
            dst = randSwVec[dstIndex];
            if(FindVecEle(possibleConnect[src], dst) && outRemainDegrees[src] > 0 && inRemainDegrees[dst] > 0){
                tempSrcVec.push_back(src);
                tempDstVec.push_back(dst);
                remainFlatEdge--;
                geneEdgeNum++;
                if(srcIndex == switches-1){
                    srcIndex = 0;
                    dstIndex = 1;
                }
                else{
                    srcIndex++;
                    dstIndex = srcIndex + 1;
                }
            }
            else{
                if(dstIndex >= switches-1){
                    if(srcIndex == switches -1)
                        break;
                    else{
                        srcIndex++;
                        dstIndex = srcIndex+1;
                    }
                }
                else{
                    dstIndex++;
                }
            }
        }
        if(geneEdgeNum == flatEdgeLayerNum[layerCount]){
            layerCount++;
            for(int i = 0; i < flatEdgeLayerNum[layerCount-1]; i++){
                RemoveVecEle(possibleConnect[tempSrcVec[i]], tempDstVec[i]);
                RemoveVecEle(possibleConnect[tempDstVec[i]], tempSrcVec[i]);
                outRemainDegrees[tempSrcVec[i]]--;
                inRemainDegrees[tempDstVec[i]]--;
            }
        }
        else
            deadCycleBreak++;
        if(deadCycleBreak > 1e2){
            std::cerr <<  "Can't construct, please change the rand seed!" << std::endl;
            exit(1);
        }
    }
    std::cout <<"Flat edge constructed!\n";
}
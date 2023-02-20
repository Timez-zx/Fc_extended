#include "graph_tag.h"

ReverseTaggerGraph::ReverseTaggerGraph(const std::string &filePath){
    FILE* ifs = fopen(filePath.c_str(), "r");
    fseek(ifs, 0, SEEK_END);
    int lenSize = ftell(ifs);
    rewind(ifs);
    int realSize = lenSize/4;
    int *allData = new int[realSize];
    int state = fread(allData, sizeof(int), realSize, ifs);
    int maxData = allData[realSize-3]*allData[realSize-2]*allData[realSize-1];
    heads = new TaggerNodeId[maxData];
    memset(heads, 0xff, maxData*sizeof(TaggerNodeId));
    edges = new SPLink[(realSize-3)/2];
    maxNode = maxData;
    edgeNum = (realSize-3)/2;
    int src,dst;
    for(int i = 0; i < (realSize-3)/2; i++){
        src = allData[2*i];
        dst = allData[2*i+1];
        edges[i].toNode = src;
        edges[i].nextEdgeIdex = heads[dst];
        heads[dst] = i;
    }
    fclose(ifs);
    delete allData;
    allData = NULL;
}


ReverseTaggerGraph::~ReverseTaggerGraph(){
    delete heads;
    heads = NULL;
    delete edges;
    edges = NULL;

}
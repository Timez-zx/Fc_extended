#ifndef GRAPH_TAG_
#define GRAPH_TAG_

#include <iostream>
#include <vector>
#include <cstring>
#include <string>

using TaggerNodeId = int;


typedef struct _SPLink{
    TaggerNodeId toNode;
    int nextEdgeIdex;
} SPLink;

class ReverseTaggerGraph{
    public:
        explicit ReverseTaggerGraph(const std::string &filePath);
        ~ReverseTaggerGraph();
        const int &getMaxNode(){
            return maxNode;
        }
        const int &getEdgeNum(){
            return edgeNum;
        }
    
    private:
        TaggerNodeId *heads;
        SPLink *edges;
        int maxNode;
        int edgeNum;
};


class ContractTaggerGraph{
    public:
        explicit ContractTaggerGraph(const int &maxNode, const int &edgeNum);
        ~ContractTaggerGraph();

    
    private:
        TaggerNodeId *heads;
        SPLink *edges;
};






#endif

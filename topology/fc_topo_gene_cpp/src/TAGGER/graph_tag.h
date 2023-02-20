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
    TaggerNodeId srcNode;
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

        const int &getHead(const int &headIndex){
            return heads[headIndex];
        }

        const SPLink &getEdge(const int &edgeIndex){
            return edges[edgeIndex];
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

        inline void AddEdge(const int &src, const int &dst){
            edges[edgesCount].toNode = dst;
            edges[edgesCount].nextEdgeIdex = heads[src];
            edges[edgesCount].srcNode = src;
            heads[src] = edgesCount++;
        }

        inline void DeleEdge(){
            edgesCount--;
            heads[edges[edgesCount].srcNode] = edges[edgesCount].nextEdgeIdex;
        }

        const bool DetectCycle(const int& start, const int& end);

    private:
        TaggerNodeId *heads;
        SPLink *edges;
        int edgesCount;
};


class SearchMinTag{
    public:
        SearchMinTag(const std::string &filePath){
            rGraph = new ReverseTaggerGraph(filePath);
            MinTag();
        }

        ~SearchMinTag(){
            delete rGraph;
            rGraph = NULL;
        }

        const int& GetMinTag(){
            return minTag;
        }

        void MinTag();
    private:
        ReverseTaggerGraph *rGraph;
        int minTag;
        
};







#endif

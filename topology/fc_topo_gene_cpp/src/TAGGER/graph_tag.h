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

        const int &GetMaxNode(){
            return maxNode;
        }

        const int &GetEdgeNum(){
            return edgeNum;
        }

        const int &GetmTagNum(){
            return mTag;
        }

        const int &GetSwitchNum(){
            return switchNum;
        }

        const int &GetPortNum(){
            return portNum;
        }

        const int &GetHead(const int &headIndex){
            return heads[headIndex];
        }

        const SPLink &GetEdge(const int &edgeIndex){
            return edges[edgeIndex];
        }
    
    private:
        TaggerNodeId *heads;
        SPLink *edges;
        int maxNode;
        int edgeNum;
        int switchNum;
        int portNum;
        int mTag;
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

        const int &GetHead(const int &headIndex){
            return heads[headIndex];
        }

        const SPLink &GetEdge(const int &edgeIndex){
            return edges[edgeIndex];
        }


    private:
        TaggerNodeId *heads;
        SPLink *edges;
        int edgesCount;
};


class SearchMinTag{
    public:
        SearchMinTag(const std::string &filePath){
            rGraph = new ReverseTaggerGraph(filePath);
        }

        ~SearchMinTag(){
            delete rGraph;
            rGraph = NULL;
        }


        const int MinimumTag();

        const int GetMaxTag(){
            int maxTag;
            maxTag = rGraph->GetmTagNum();
            return maxTag; 
        }


    private:
        ReverseTaggerGraph *rGraph;
        
};







#endif

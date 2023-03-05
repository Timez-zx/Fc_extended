#include "basic_gene_fc.h"


void AddEdges(std::vector<int>& heads, std::vector<Edge>& edges, int src, int dst, int &edgeCount){
    edges[edgeCount].toNode = dst;
    edges[edgeCount].nextEdgeIdex = heads[src];
    edges[edgeCount].srcNode = src;
    heads[src] = edgeCount++;
}


void DeleLastEdges(std::vector<int>& heads, std::vector<Edge>& edges, int &edgeCount){
    edgeCount--;
    heads[edges[edgeCount].srcNode] = edges[edgeCount].nextEdgeIdex;
}


bool DetectCycleStack(const std::vector<int>& heads, const std::vector<Edge>& edges, int start){
    std::vector<int> visited(heads.size());
    memset(&visited[0], 0, heads.size()*sizeof(int));
    std::stack<int> tempStack;
    int topVertex, nearVertex;
    tempStack.push(start);
    visited[start] = 1;
    while(!tempStack.empty()){
        topVertex = tempStack.top();
        tempStack.pop();
        for(int i = heads[topVertex]; i != -1; i = edges[i].nextEdgeIdex){
            nearVertex = edges[i].toNode;
            if(nearVertex == start)
                return true;
            else{
                if(!visited[nearVertex]){
                    tempStack.push(nearVertex);
                    visited[nearVertex] = 1;
                }
            }
        }
    }
    return false;
}


bool DetectCycleStackUndirect(const std::vector<int>& heads, const std::vector<Edge>& edges, int start, int end){
    std::vector<int> visited(heads.size());
    memset(&visited[0], 0, heads.size()*sizeof(int));
    std::stack<int> tempStack;
    int topVertex, nearVertex;
    tempStack.push(start);
    visited[start] = 1;
    while(!tempStack.empty()){
        topVertex = tempStack.top();
        tempStack.pop();
        for(int i = heads[topVertex]; i != -1; i = edges[i].nextEdgeIdex){
            nearVertex = edges[i].toNode;
            if(nearVertex == end)
                return true;
            else{
                if(!visited[nearVertex]){
                    tempStack.push(nearVertex);
                    visited[nearVertex] = 1;
                }
            }
        }
    }
    return false;
}
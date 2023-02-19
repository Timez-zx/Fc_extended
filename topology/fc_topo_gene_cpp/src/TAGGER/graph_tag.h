#ifndef GRAPH_TAG_
#define GRAPH_TAG_

#include <iostream>
#include <vector>
#include <string>

using TaggerNodeId = int;


typedef struct _SPLink{
    TaggerNodeId toNode;
    int nextEdgeIdex;
} SPLink;

class ReverseTaggerGraph{
    public:
        explicit ReverseTaggerGraph(const std::string &file_path);
        ~ReverseTaggerGraph(){};
    
    private:
        TaggerNodeId *heads;
        SPLink *edges;
};






#endif

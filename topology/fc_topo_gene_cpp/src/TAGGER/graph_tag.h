#ifndef GRAPH_TAG_
#define GRAPH_TAG_

#include <iostream>
#include <vector>

using TaggerNodeId = int;


typedef struct _SPLink{
    TaggerNodeId toNode;
    int nextEdgeIdex;
} SPLink;

class ReverseTaggerGraph{
    public:
        explicit ReverseTaggerGraph();
        ~ReverseTaggerGraph(){};


};






#endif

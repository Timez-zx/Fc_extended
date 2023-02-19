#ifndef GRAPH_TAG_
#define GRAPH_TAG_

#include <iostream>
#include <vector>

using SwitchId = int;
using PortId = int;
using TagId = int;

typedef struct _SPNode{
    _SPNode(SwitchId switch_in, PortId port_in, TagId tag_in, int index_in):
           switchId(switch_in), portId(port_in), tagId(tag_in), FirstIndex(index_in){}
    ~_SPNode(){}
    SwitchId switchId;
    PortId portId;
    TagId tagId;
    int FirstIndex;
} SPNode;

typedef struct _SPLink{
    _SPLink(SPNode dst_in, int index_in): 
            dstNode(dst_in), NextIndex(index_in){}
    ~_SPLink(){}
    SPNode dstNode;
    int NextIndex;
} SPLink;

class TaggerGraph{
    public:
        explicit TaggerGraph();
        ~TaggerGraph(){};


};






#endif

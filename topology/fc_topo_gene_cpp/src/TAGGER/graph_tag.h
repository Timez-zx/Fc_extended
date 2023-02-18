#ifndef GRAPH_TAG_
#define GRAPH_TAG_

#include <iostream>
#include <vector>

using SwitchId = int;
using PortId = int;
using TagId = int;
using SPLinkId = int;

typedef struct _SPNode{
    _SPNode(SwitchId switch_in, PortId port_in, TagId tag_in):
           switchId(switch_in), portId(port_in), tagId(tag_in){}
    ~_SPNode(){}
    SwitchId switchId;
    PortId portId;
    TagId tagId;
} SPNode;

typedef struct _SPLink{
    _SPLink(SPNode src_in, SPNode dst_in, SPLinkId id_in): 
           srcNode(src_in), dstNode(dst_in), splinkId(id_in){}
    ~_SPLink(){}
    SPNode srcNode;
    SPNode dstNode;
    SPLinkId splinkId;
} SPLink;

class TaggerGraph{
    public:
        explicit TaggerGraph();
        ~TaggerGraph(){};

    private:
        std::vector<SPLink> spLinks;

};






#endif

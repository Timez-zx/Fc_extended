#ifndef GRAPH_TAG_
#define GRAPH_TAG_

#include <iostream>

using SwitchId = int;
using PortId = int;
using TagId = int;

typedef struct _SPNode{
    SwitchId switchId;
    PortId portId;
    TagId tagId;
}SPNode;




#endif

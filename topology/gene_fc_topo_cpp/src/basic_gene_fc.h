#ifndef BASIC_GENE_FC_H_
#define BASIC_GENE_FC_H_
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

typedef struct _SwNode
{
    int swLabel;
    int layerLabel;
    _SwNode(int swLabelIn, int layerLabelIn):swLabel(swLabelIn), layerLabel(layerLabelIn){}
} SwNode;

typedef struct _SwLink
{
    SwNode srcNode;
    SwNode dstNode;
    _SwLink(const SwNode& srcNodeIn, const SwNode& dstNodeIn): srcNode(srcNodeIn), dstNode(dstNodeIn){}
} SwLink;


class BasicGeneFc{
    public:
        virtual ~BasicGeneFc() {}
        virtual void GeneTopo() = 0;
};

#endif  // BASIC_GENE_FC_H_
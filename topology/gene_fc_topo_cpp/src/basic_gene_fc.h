#ifndef BASIC_GENE_FC_H_
#define BASIC_GENE_FC_H_
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <set>
#include <vector>
#include <numeric>
#include <random>
#include <stack>
#include <thread>
#include <cstring>
#include <algorithm>
#include <unordered_map>
#include "utils.h"
#include "gurobi_c++.h"
#include "KSP_PROZ/shortest_path.h"

typedef struct _SwNode
{
    int swLabel;
    int layerLabel;
    _SwNode(int swLabelIn, int layerLabelIn):swLabel(swLabelIn), layerLabel(layerLabelIn){}
} SwNode;

typedef struct _SwNodePort
{
    int swLabel;
    int portLabel;
    _SwNodePort(int swLabelIn, int portLabelIn):swLabel(swLabelIn), portLabel(portLabelIn){}
} SwNodePort;

typedef struct _SwLinkPort
{
    SwNodePort srcNode;
    SwNodePort dstNode;
    _SwLinkPort(const SwNodePort& srcNodeIn, const SwNodePort& dstNodeIn): srcNode(srcNodeIn), dstNode(dstNodeIn){}
} SwLinkPort;

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


typedef struct _Edge
{
    int nextEdgeIdex;
    int toNode;
    int srcNode;
} Edge;

typedef struct _Pair
{
    int src;
    int dst;
    _Pair(int srcIn, int dstIn): src(srcIn), dst(dstIn){}
} Pair;

bool DetectCycleStack(const std::vector<int>& heads, const std::vector<Edge>& edges, int start);

bool DetectCycleStackUndirect(const std::vector<int>& heads, const std::vector<Edge>& edges, int start, int end);

void AddEdges(std::vector<int>& heads, std::vector<Edge>& edges, int src, int dst, int &edgeCount);

void DeleLastEdges(std::vector<int>& heads, std::vector<Edge>& edges, int &edgeCount);
#endif  // BASIC_GENE_FC_H_
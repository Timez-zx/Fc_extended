#ifndef FC_WITH_FLAT_EDGE_H_
#define FC_WITH_FLAT_EDGE_H_

#include "basic_gene_fc.h"

class FcWithFlatEdge: public BasicGeneFc{
    public:
        ~FcWithFlatEdge(){};
        void GeneTopo() override;
};


#endif  // FC_WITH_FLAT_EDGE_H_
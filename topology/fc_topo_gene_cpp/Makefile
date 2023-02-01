CFLAGS_M= -O3 -P -std=c++17
CFLAGS_U= -O3 -P -std=c++17 
FILENAME=fc_topo_all_route.cpp fc_edge_disjoin_route.cpp fc_edge_disjoin_cost_route.cpp  main.cpp
OUT=main

ORTOOL_MAC=/Users/zhangxiao/Documents/lab/Rdma-cbd/fc_extend/or-tools
ORTOOL_LIB_M=$(ORTOOL_MAC)/lib/
ORTOOL_INCLUDE_M=$(ORTOOL_MAC)/include/

ORTOOL_UBUNTU=~/or-tools
ORTOOL_LIB_U=$(ORTOOL_UBUNTU)/lib/
ORTOOL_INCLUDE_U=$(ORTOOL_UBUNTU)/include/

GUROBI_UBUNTU=~/Optimizer/gurobi952/linux64
GUROBI_LIB_U=$(GUROBI_UBUNTU)/lib/
GUROBI_INCLUDE_U=$(GUROBI_UBUNTU)/include/

CLEAN_PATH=./all_graph_infor/*  ./all_graph_route/* 

default: $(FILENAME)
	g++  $(CFLAGS_U) $(FILENAME) -o $(OUT) -I $(ORTOOL_INCLUDE_U) -I $(GUROBI_INCLUDE_U) -L $(ORTOOL_LIB_U) -L $(GUROBI_LIB_U) -lortools -lpthread -lgurobi_c++ -lgurobi95

MAC: $(FILENAME)
	g++  $(CFLAGS_M) $(FILENAME) -o $(OUT) -I $(ORTOOL_INCLUDE_M) -L $(ORTOOL_LIB_M)  -lortools -lpthread

clean:
	$(RM) $(OUT)
	$(RM) -r $(CLEAN_PATH)

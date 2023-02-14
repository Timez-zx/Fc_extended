CFLAGS_U= -O3 -P -std=c++17 

ALL_SRC = $(wildcard src/*.cpp) $(wildcard src/KSP/*.cpp)
ALL_HEADER = $(wildcard src/*.h) $(wildcard src/KSP/*.h)

OUT=main

ORTOOL_UBUNTU=~/or-tools
ORTOOL_LIB_U=$(ORTOOL_UBUNTU)/lib/
ORTOOL_INCLUDE_U=$(ORTOOL_UBUNTU)/include/

GUROBI_UBUNTU=~/Optimizer/gurobi952/linux64
GUROBI_LIB_U=$(GUROBI_UBUNTU)/lib/
GUROBI_INCLUDE_U=$(GUROBI_UBUNTU)/include/

CLEAN_PATH=data/all_graph_infor/*  data/all_graph_route/*  data/worst_flow_infor/* data/topo_infor/*
CLEAN_PATHK=data/all_graph_route_ksp/*

default: $(ALL_SRC) $(ALL_HEADER) main.cpp
	g++  $(CFLAGS_U) $(ALL_SRC) main.cpp -o $(OUT) -I $(ORTOOL_INCLUDE_U) -I $(GUROBI_INCLUDE_U) -L $(ORTOOL_LIB_U) -L $(GUROBI_LIB_U) -lortools -lpthread -lgurobi_c++ -lgurobi95


clean:
	$(RM) $(OUT)
	$(RM) -r $(CLEAN_PATH)

cleank:
	$(RM) -r $(CLEAN_PATHK)

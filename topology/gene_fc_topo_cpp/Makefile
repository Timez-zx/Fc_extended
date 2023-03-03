CFLAGS_U= -O3 -P -std=c++17

ALL_SRC = $(wildcard src/*.cpp) $(wildcard src/KSP_PROZ/*.cpp) 
ALL_HEADER = $(wildcard src/*.h)  $(wildcard src/KSP_PROZ/*.h) 

OUT=main

CLEANPATH=data/topo_infor/* data/all_graph_route_ksp/*

GUROBI_UBUNTU=~/Optimizer/gurobi952/linux64
GUROBI_LIB_U=$(GUROBI_UBUNTU)/lib/
GUROBI_INCLUDE_U=$(GUROBI_UBUNTU)/include/

default: $(ALL_SRC) $(ALL_HEADER) main.cpp
	g++  $(CFLAGS_U) $(ALL_SRC) main.cpp -o $(OUT) -I $(GUROBI_INCLUDE_U) -L $(GUROBI_LIB_U) -lgurobi_c++ -lgurobi95


clean:
	$(RM) $(OUT) 
	$(RM) -r $(CLEANPATH)



CFLAGS_U= -O3 -P -std=c++17

ALL_SRC = $(wildcard src/*.cpp) $(wildcard src/KSP_PROZ/*.cpp) 
ALL_HEADER = $(wildcard src/*.h)  $(wildcard src/KSP_PROZ/*.h) 

OUT=main

CLEANPATH=data/topo_infor/* data/all_graph_route_ksp/* data/worst_flow_infor/*

GUROBI_UBUNTU=~/Optimizer/gurobi952/linux64
GUROBI_LIB_U=$(GUROBI_UBUNTU)/lib/
GUROBI_INCLUDE_U=$(GUROBI_UBUNTU)/include/

GUROBI_MAC=/Library/gurobi1001/macos_universal2
GUROBI_LIB_M=$(GUROBI_MAC)/lib/
GUROBI_INCLUDE_M=$(GUROBI_MAC)/include/

default: $(ALL_SRC) $(ALL_HEADER) main.cpp
	g++  $(CFLAGS_U) $(ALL_SRC) main.cpp -o $(OUT) -I $(GUROBI_INCLUDE_U) -L $(GUROBI_LIB_U) -lgurobi_c++ -lgurobi95


mac: $(ALL_SRC) $(ALL_HEADER) main.cpp
	g++  $(CFLAGS_U) $(ALL_SRC) main.cpp -o $(OUT) -I $(GUROBI_INCLUDE_M) -L $(GUROBI_LIB_M) -lgurobi_c++ -lgurobi100


clean:
	$(RM) $(OUT) 
	$(RM) -r $(CLEANPATH)



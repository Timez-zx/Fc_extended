CFLAGS_U= -O3 -P -std=c++17

ALL_SRC = $(wildcard src/*.cpp) $(wildcard src/KSP_PROZ/*.cpp)  $(wildcard src/TAGGER/*.cpp)
ALL_HEADER = $(wildcard src/*.h) $(wildcard src/KSP_PROZ/*.h)   $(wildcard src/TAGGER/*.h)

ALL_SRC_T = $(wildcard src/TAGGER/*.cpp)
ALL_HEADER_T = $(wildcard src/TAGGER/*.h)

ALL_SRC_M = $(wildcard src/fc_t*.cpp)  $(wildcard src/fc_b*.cpp)  $(wildcard src/u*.cpp) $(wildcard src/KSP_PROZ/*.cpp)  $(wildcard src/TAGGER/*.cpp)
ALL_HEADER_M = $(wildcard src/fc_t*.h) $(wildcard src/fc_b*.h)  $(wildcard src/u*.h) $(wildcard src/KSP_PROZ/*.h)   $(wildcard src/TAGGER/*.h)

OUT=main

ORTOOL_UBUNTU=~/or-tools
ORTOOL_LIB_U=$(ORTOOL_UBUNTU)/lib/
ORTOOL_INCLUDE_U=$(ORTOOL_UBUNTU)/include/

GUROBI_UBUNTU=~/Optimizer/gurobi952/linux64
GUROBI_LIB_U=$(GUROBI_UBUNTU)/lib/
GUROBI_INCLUDE_U=$(GUROBI_UBUNTU)/include/

GUROBI_MAC=/Library/gurobi1001/macos_universal2
GUROBI_LIB_M=$(GUROBI_MAC)/lib/
GUROBI_INCLUDE_M=$(GUROBI_MAC)/include/

CLEAN_PATH=data/all_graph_infor/*  data/all_graph_route/*  data/worst_flow_infor/* data/topo_infor/* data/all_graph_route_ksp/* 
CLEAN_PATHK=data/tagger_infor/*

default: $(ALL_SRC) $(ALL_HEADER) main.cpp
	g++  $(CFLAGS_U) $(ALL_SRC) main.cpp -o $(OUT) -I $(ORTOOL_INCLUDE_U) -I $(GUROBI_INCLUDE_U) -L $(ORTOOL_LIB_U) -L $(GUROBI_LIB_U) -lortools -lpthread -lgurobi_c++ -lgurobi95


mac: $(ALL_SRC_M) $(ALL_HEADER_M) main.cpp
	g++  $(CFLAGS_U) $(ALL_SRC_M) main.cpp -o $(OUT) -I $(GUROBI_INCLUDE_M) -L $(GUROBI_LIB_M) -lgurobi_c++ -lgurobi100

tagger: $(ALL_SRC_T) $(ALL_HEADER_T) main_tagger.cpp
	g++ $(CFLAGS_U) $(ALL_SRC_T) main_tagger.cpp -o $(OUT)

clean:
	$(RM) $(OUT)
	$(RM) -r $(CLEAN_PATH)

cleank:
	$(RM) -r $(CLEAN_PATHK)


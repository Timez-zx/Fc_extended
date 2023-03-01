CFLAGS_U= -O3 -P -std=c++17

ALL_SRC = $(wildcard src/*.cpp) 
ALL_HEADER = $(wildcard src/*.h)

OUT=main

GUROBI_UBUNTU=~/Optimizer/gurobi952/linux64
GUROBI_LIB_U=$(GUROBI_UBUNTU)/lib/
GUROBI_INCLUDE_U=$(GUROBI_UBUNTU)/include/

default: $(ALL_SRC) $(ALL_HEADER) main.cpp
	g++  $(CFLAGS_U) $(ALL_SRC) main.cpp -o $(OUT) 


clean:
	$(RM) $(OUT)



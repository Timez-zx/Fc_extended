CFLAGS= -O3 -P -std=c++11
FILENAME=fc_topo_all_route.cpp fc_edge_disjoin_route.cpp main.cpp
OUT=main
ORTOOL_MAC=/Users/zhangxiao/Documents/lab/Rdma-cbd/fc_extend/or-tools
ORTOOL_LIB_M=$(ORTOOL_MAC)/lib/
ORTOOL_INCLUDE_M=$(ORTOOL_MAC)/include/

ORTOOL_UBUNTU=~/or-tools
ORTOOL_LIB_U=$(ORTOOL_UBUNTU)/lib/
ORTOOL_INCLUDE_U=$(ORTOOL_UBUNTU)/include/

default: $(FILENAME)
	g++  $(CFLAGS) $(FILENAME) -o $(OUT) -I $(ORTOOL_INCLUDE_M) -L $(ORTOOL_LIB_M) -lortools -lpthread

Ubuntu: $(FILENAME)
	g++  $(CFLAGS) $(FILENAME) -o $(OUT) -I $(ORTOOL_INCLUDE_U) -L $(ORTOOL_LIB_U) -lortools -lpthread

clean:
	$(RM) $(OUT)

CFLAGS_U= -O3 -P -std=c++17 

ALL_SRC = $(wildcard *.cpp)
ALL_HEADER = $(wildcard *.h)

OUT=main

default: $(ALL_SRC) $(ALL_HEADER) 
	g++  $(CFLAGS_U) $(ALL_SRC) -o $(OUT) 


clean:
	$(RM) $(OUT)

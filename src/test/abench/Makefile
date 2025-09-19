.PHONY: test all clean

CXXFLAGS=-O3 -ffast-math -funroll-loops -march=native -Wall -Wextra -std=c++11

all: Makefile AlgoRank.cpp
	$(CXX) $(CXXFLAGS) -o AlgoRank AlgoRank.cpp

test: all
	./AlgoRank

clean:
	rm -f AlgoRank

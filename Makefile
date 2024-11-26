CXX = g++
DEPEND = -I	./include/eigen/ -I	.
CXXFLAGS_11 = -std=c++11 -O3
CXXFLAGS_14 = -std=c++14 -O3
CXXFLAGS_17 = -std=c++17 -O3

all:	main	optimum	exactsm	approxisc	random_walk	approxisc

main:	code/main.cpp code/graph_utils.cpp
	$(CXX)	$(CXXFLAGS_17)	$(DEPEND)	code/main.cpp	code/graph_utils.cpp	-o	output/main		
optimum:	code/optimum.cpp
	$(CXX)	$(CXXFLAGS_14)	$(DEPEND)	code/optimum.cpp	-o	output/optimum	
exactsm:	code/exactsm.cpp
	$(CXX)	$(CXXFLAGS_14)	$(DEPEND)	code/exactsm.cpp	-o	output/exactsm
approxisc:	code/approxisc.cpp
	$(CXX)	$(CXXFLAGS_17)	$(DEPEND)	code/approxisc.cpp	-o	output/approxisc
fasticm:	code/fasticm.cpp
	$(CXX)	$(CXXFLAGS_17)	$(DEPEND)	code/fasticm.cpp	-o	output/fasticm
random_walk:	code/random_walk.cpp
	$(CXX)	$(CXXFLAGS_14)	$(DEPEND)	-pthread	code/random_walk.cpp	-o	output/random_walk
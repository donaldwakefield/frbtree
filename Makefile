CXXFLAGS = -I. -I/scratch1/boost/include/boost-1_38 -O3
tree: rb-tree.h main.C
	$(CXX) $(CXXFLAGS) main.C -o tree
tree2: rb-tree.h main2.C
	$(CXX) $(CXXFLAGS) main2.C -o tree2
treej: rb-treej.h main3.C
	$(CXX) $(CXXFLAGS) main3.C -o treej
trees: set_main.C
	$(CXX) $(CXXFLAGS) set_main.C -o trees
treep: rb-treep.h main3.C
	$(CXX) $(CXXFLAGS) main3.C -o treep
all: tree tree2 treej
clean:
	rm tree tree2 treej transcript core* >& /dev/null

CXXFLAGS = -I. -I/scratch1/boost/include/boost-1_38
tree: rb-tree.h main.C
	$(CXX) $(CXXFLAGS) main.C -o tree
tree2: rb-tree.h main2.C
	$(CXX) $(CXXFLAGS) main2.C -o tree2
all: tree tree2
clean:
	rm tree tree2 transcript core* >& /dev/null

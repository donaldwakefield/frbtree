# first hardpath is to my workstation at work, second to that at home
CXXFLAGS = -I. -I/scratch1/boost/include/boost-1_38 -I/usr/local/include/boost-1_38/ -O3
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
all: tree tree2 treej trees treep
clean:
	rm tree tree2 treej transcript core* >& /dev/null

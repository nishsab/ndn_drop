TARGETS = ndn_drop
SOURCES = main.cpp NeighborList.cpp Utils.cpp NeighborListRequestor.cpp NeighborListName.cpp NeighborListRepo.cpp
CFLAGS = `pkg-config --cflags --libs libndn-cxx`
STATIC_LIBS = -l mongoose
all: clean $(TARGETS)

ndn_drop: main.cpp $(SOURCES)
	g++ -std=c++14 $(STATIC_LIBS) -o $@ $(SOURCES) -Wall -ggdb -O0 $(CFLAGS)

clean:
	rm -rf *.dSYM
	rm -rf $(TARGETS)
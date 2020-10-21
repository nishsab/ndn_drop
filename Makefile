TARGETS = aircon NeighborListRequestor newcon ndn_drop
SOURCES = main.cpp NeighborList.cpp Utils.cpp NeighborListRequestor.cpp NeighborListName.cpp NeighborListRepo.cpp
CFLAGS = `pkg-config --cflags --libs libndn-cxx`
STATIC_LIBS = -l mongoose
all: $(TARGETS)

newcon: newcon.cpp
	g++ -std=c++14 $< -o $@ -Wall -ggdb -O0 $(CFLAGS)

aircon: aircon.cpp
	g++ -std=c++14 $< -o $@ -Wall -ggdb -O0 $(CFLAGS)

NeighborListRequestor: NeighborListRequestor.cpp
	g++ -std=c++14 $< -o $@ -Wall -ggdb -O0 $(CFLAGS)

ndn_drop: main.cpp $(SOURCES)
	g++ -std=c++14 $(STATIC_LIBS) -o $@ $(SOURCES) -Wall -ggdb -O0 $(CFLAGS)

clean:
	rm -rf *.dSYM
	rm -rf $(TARGETS)
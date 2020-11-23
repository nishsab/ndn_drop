TARGETS = ndn_drop
SOURCES = main.cpp NeighborList.cpp Utils.cpp NeighborListRequestor.cpp NeighborListName.cpp \
NeighborListRepo.cpp Conf.cpp FileName.cpp FileRepo.cpp FileRequestor.cpp FileDownloader.cpp \
file_manager/DirectoryManager.cpp file_manager/DirectoryMonitor.cpp file_manager/FileInfo.cpp \
file_manager/PacketEncoder.cpp PracticalSocket.cpp file_manager/PacketSender.cpp file_manager/MetadataConverter.cpp
CFLAGS = `pkg-config --cflags --libs libndn-cxx libndn-nac`
STATIC_LIBS = -l mongoose
all: clean $(TARGETS)

ndn_drop: main.cpp $(SOURCES)
	g++ -std=c++14 $(STATIC_LIBS) -o $@ $(SOURCES) -Wall -ggdb -O0 $(CFLAGS)

clean:
	rm -rf *.dSYM
	rm -rf $(TARGETS)
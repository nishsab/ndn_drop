cmake_minimum_required(VERSION 3.16)
project(ndn_drop)

set(CMAKE_CXX_STANDARD 14)

add_executable(ndn_drop NeighborList.cpp NeighborList.h main.cpp Utils.cpp Utils.h NeighborListRequestor.cpp NeighborListRequestor.h NeighborListName.cpp NeighborListName.h NeighborListRepo.cpp NeighborListRepo.h Config.h Conf.cpp Conf.h DirectoryCrawler.cpp DirectoryCrawler.h FileName.cpp FileName.h FileRepo.cpp FileRepo.h IdentityName.cpp IdentityName.h FileRequestor.cpp FileRequestor.h FileDownloader.cpp FileDownloader.h)

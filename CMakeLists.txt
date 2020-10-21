cmake_minimum_required(VERSION 3.16)
project(ndn_drop)

set(CMAKE_CXX_STANDARD 14)

add_executable(ndn_drop aircon.cpp aircon.cpp NeighborListRequestor_old.cpp newcon.cpp NeighborList.cpp NeighborList.h main.cpp build/Utils.cpp build/Utils.h Utils.cpp Utils.h NeighborListRequestor.cpp NeighborListRequestor.h NeighborListName.cpp NeighborListName.h NeighborListRepo.cpp NeighborListRepo.h Config.h)

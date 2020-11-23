cmake_minimum_required(VERSION 3.16)
project(ndn_drop)

set(CMAKE_CXX_STANDARD 14)

add_executable(ndn_drop NeighborList.cpp NeighborList.h main.cpp Utils.cpp Utils.h NeighborListRequestor.cpp NeighborListRequestor.h NeighborListName.cpp NeighborListName.h NeighborListRepo.cpp NeighborListRepo.h Conf.cpp Conf.h FileName.cpp FileName.h FileRepo.cpp FileRepo.h FileRequestor.cpp FileRequestor.h FileDownloader.cpp FileDownloader.h testing.cpp consumer.cpp PracticalSocket.cpp PracticalSocket.h file_manager/DirectoryManager.cpp file_manager/DirectoryManager.h file_manager/DirectoryMonitor.cpp file_manager/DirectoryMonitor.h file_manager/FileInfo.cpp file_manager/FileInfo.h file_manager/PacketEncoder.cpp file_manager/PacketSender.cpp file_manager/PacketSender.h file_manager/MetadataConverter.cpp file_manager/MetadataConverter.h nac/decryptor2.cpp nac/decryptor2.h nac/EncryptedContent.cpp nac/EncryptedContent.h nac/common.cpp nac/common.h nac/encryptor.cpp nac/encryptor.h file_manager/PacketEncoder.h)

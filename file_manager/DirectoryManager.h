//
// Created by NIshant Sabharwal on 11/12/20.
//

#ifndef NDN_DROP_DIRECTORYMANAGER_H
#define NDN_DROP_DIRECTORYMANAGER_H

#include <string>
#include <thread>
#include "DirectoryMonitor.h"
#include "PacketEncoder.h"

using namespace std;

class DirectoryManager {
public:
    DirectoryManager(string directoryPath,
                     int blockSize,
                     string prefix,
                     string repoHostName,
                     int repoPort,
                     string certificateName,
                     string pibLocator,
                     string tpmLocator,
                     string nacAccessPrefix,
                     string nacCkPrefix,
                     string schemaConfPath);
    string getFileMetaDataJson();

private:
    DirectoryMonitor *directoryMonitor;
    string repoHostName;
    int repoPort;
    void threadRunner();
    void stop();
    thread directoryManagerThread;
    bool running;
    PacketEncoder packetEncoder;
};


#endif //NDN_DROP_DIRECTORYMANAGER_H

//
// Created by NIshant Sabharwal on 11/12/20.
//

#ifndef NDN_DROP_DIRECTORYMANAGER_H
#define NDN_DROP_DIRECTORYMANAGER_H

#include <string>
#include <thread>
#include "DirectoryMonitor.h"
#include "../SecurityPackage.h"

using namespace std;

class DirectoryManager {
public:
    DirectoryManager(string directoryPath, int blockSize, string prefix, string repoHostName, int repoPort, SecurityPackage *securityPackage, string certificateName);
    string getFileMetaDataJson();

private:
    DirectoryMonitor *directoryMonitor;
    string repoHostName;
    int repoPort;
    void threadRunner();
    void stop();
    thread directoryManagerThread;
    bool running;
    SecurityPackage *securityPackage;
    string certificateName;
};


#endif //NDN_DROP_DIRECTORYMANAGER_H

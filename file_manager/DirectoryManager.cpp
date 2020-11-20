//
// Created by NIshant Sabharwal on 11/12/20.
//

#include <vector>
#include <iostream>
#include "DirectoryManager.h"
#include "DirectoryMonitor.h"
#include "PacketEncoder.h"
#include "PacketSender.h"

DirectoryManager::DirectoryManager(string directoryPath, int blockSize, string prefix, string repoHostName, int repoPort, SecurityPackage *securityPackage, string certificateName)
{
    directoryMonitor = new DirectoryMonitor(directoryPath, blockSize, prefix);
    this->repoHostName = repoHostName;
    this->repoPort = repoPort;
    running = true;
    directoryManagerThread = thread(&DirectoryManager::threadRunner, this);
    this->securityPackage = securityPackage;
    this->certificateName = certificateName;
}

void DirectoryManager::threadRunner() {
    PacketEncoder packetEncoder(certificateName, securityPackage);
    PacketSender packetSender(repoHostName, repoPort);
    vector<FileInfo> fileInfos = directoryMonitor->checkForNewFiles();
    /*vector<const Data> dataPackets = packetEncoder.encodePackets(fileInfos);
    if (!dataPackets.empty()) {
        packetSender.sendPackets(dataPackets);
    }*/
    directoryMonitor->persist(fileInfos);
    while (running) {
        sleep(5);
    }
}

string DirectoryManager::getFileMetaDataJson() {
    return directoryMonitor->getFileMetadatajson();
}

void DirectoryManager::stop() {
    running = false;
    directoryManagerThread.join();
}


//
// Created by NIshant Sabharwal on 11/12/20.
//

#include <vector>
#include <iostream>
#include "DirectoryManager.h"
#include "DirectoryMonitor.h"
#include "PacketEncoder.h"
#include "PacketSender.h"
#include "../Utils.h"

DirectoryManager::DirectoryManager(string directoryPath,
                                   int blockSize,
                                   string prefix,
                                   string repoHostName,
                                   int repoPort,
                                   string certificateName,
                                   string pibLocator,
                                   string tpmLocator,
                                   string nacAccessPrefix,
                                   string nacCkPrefix,
                                   string schemaConfPath)
: packetEncoder(certificateName,
                pibLocator,
                tpmLocator,
                nacAccessPrefix,
                nacCkPrefix,
                schemaConfPath)
{
    directoryMonitor = new DirectoryMonitor(directoryPath, blockSize, prefix);
    this->repoHostName = repoHostName;
    this->repoPort = repoPort;
    running = true;
    directoryManagerThread = thread(&DirectoryManager::threadRunner, this);
    Utils::logf("Directory manager started\n");
}

void DirectoryManager::threadRunner() {
    PacketSender packetSender(repoHostName, repoPort);
    while (running) {
        vector<FileInfo> fileInfos = directoryMonitor->checkForNewFiles();
        vector<const Data> dataPackets = packetEncoder.encodePackets(fileInfos);
        if (!dataPackets.empty()) {
            packetSender.sendPackets(dataPackets);
        }
        directoryMonitor->persist(fileInfos);
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


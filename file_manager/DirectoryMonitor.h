//
// Created by NIshant Sabharwal on 11/12/20.
//

#ifndef NDN_DROP_DIRECTORYMONITOR_H
#define NDN_DROP_DIRECTORYMONITOR_H

#include "FileInfo.h"
#include <string>
#include <unordered_map>
#include <mutex>

using namespace std;

class DirectoryMonitor {
public:
    DirectoryMonitor(string directoryPath, int blockSize, string prefix);
    void persist(vector<FileInfo> fileInfos);

    vector<FileInfo> checkForNewFiles();
    string getFileMetadatajson();
private:
    string directoryPath;
    int blockSize;
    string prefix;
    mutex lock;
    unordered_map<string, FileInfo> trackedFiles;
    vector<FileInfo> getTrackedFiles();
};


#endif //NDN_DROP_DIRECTORYMONITOR_H

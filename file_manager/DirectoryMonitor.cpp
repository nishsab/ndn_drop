//
// Created by NIshant Sabharwal on 11/12/20.
//

#include <vector>
#include <dirent.h>
#include <iostream>
#include <sys/stat.h>
#include "DirectoryMonitor.h"
#include "MetadataConverter.h"
#include "../Utils.h"
#include <math.h>
#include <mutex>

vector<string> getFiles(string directoryPath);
long getFileModTime(string relativePath);
long getFileSize(string relativePath);

DirectoryMonitor::DirectoryMonitor(string directoryPath, int blockSize, string prefix) {
    this->directoryPath = directoryPath;
    this->prefix = prefix;
    this->blockSize = blockSize;
}

vector<FileInfo> DirectoryMonitor::checkForNewFiles() {
    Utils::logf("DirectoryMonitor::checkForNewFiles: Checking for new files in %s\n", directoryPath.c_str());
    vector<string> filenames = getFiles(directoryPath);
    vector<FileInfo> fileInfos;
    lock.lock();
    for (string filename : filenames) {
        string relativePath = directoryPath + "/" + filename;
        long modTime = getFileModTime(relativePath);
        FileInfo fileInfo = FileInfo(filename, relativePath, prefix, blockSize);
        if (trackedFiles.find(filename) != trackedFiles.end()) {
            fileInfo = trackedFiles[filename];
        }
        //FileInfo fileInfo = trackedFiles.emplace(std::pair<string, FileInfo>(filename, FileInfo(filename, relativePath, prefix, blockSize))).first->second;
        if (modTime > fileInfo.modificationTime) {
            fileInfo.modificationTime = modTime;
            fileInfo.size = getFileSize(relativePath);
            fileInfo.numSegs = ceil(fileInfo.size / (double) blockSize);
            Utils::logf("DirectoryMonitor::checkForNewFiles: Found a new file: %s\n", fileInfo.toString().c_str());
            fileInfos.push_back(fileInfo);
        }

    }
    lock.unlock();
    return fileInfos;
}

string DirectoryMonitor::getFileMetadatajson() {
    vector<FileInfo> fileInfos = getTrackedFiles();
    cout << "getFileMetadatajson: " << fileInfos.size() << endl;
    return MetadataConverter::buildJson(fileInfos);
}

void DirectoryMonitor::persist(vector<FileInfo> fileInfos) {
    cout << "inserting " << fileInfos.size() << endl;
    cout << trackedFiles.size() << endl;
    lock.lock();
    for (FileInfo fileInfo : fileInfos) {
        trackedFiles[fileInfo.filename] = fileInfo;
    }
    lock.unlock();
    cout << "size" << trackedFiles.size() << endl;
}

vector<FileInfo> DirectoryMonitor::getTrackedFiles() {
    vector<FileInfo> fileInfos;
    lock.lock();
    for (auto const& it : trackedFiles) {
        fileInfos.push_back(it.second);
    }
    lock.unlock();
    cout << "found: " << fileInfos.size() << endl;
    return fileInfos;
}

long getFileModTime(string relativePath) {
    struct stat result = {};
    long mod_time = 0;
    if(stat(relativePath.c_str(), &result)==0)
    {
        mod_time = result.st_mtime;
    }
    return mod_time;
}

long getFileSize(string relativePath) {
    struct stat result;
    long size = 0;
    if(stat(relativePath.c_str(), &result)==0)
    {
        size = result.st_size;
    }
    return size;
}

vector<string> getFiles(string directoryPath) {
    vector<string> filenames;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (directoryPath.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
                filenames.push_back(ent->d_name);
            }
        }
        closedir (dir);
    }
    return filenames;
}
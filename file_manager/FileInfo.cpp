//
// Created by NIshant Sabharwal on 11/12/20.
//

#include <iostream>
#include "FileInfo.h"
#include "../Utils.h"

FileInfo::FileInfo() {
};

FileInfo::FileInfo(string filename, string relativePath, string prefix, int blockSize) {
    this->filename = filename;
    this->relativePath = relativePath;
    this->prefix = prefix;
    this->blockSize = blockSize;
}

Name FileInfo::getNdnName() {
    return Name(prefix).append(filename).appendVersion(modificationTime);
}

string FileInfo::toString() {
    ostringstream stream;
    stream << relativePath << ", " << filename << ", " << prefix << ", " << blockSize << ", " << size << ", " << numSegs << ", " << modificationTime;
    return stream.str();
}


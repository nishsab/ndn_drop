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

// get_file?ndn_name=/ndn/drop/nishant/laptop/testing.cpp/%FD_%BB%05r&num_blocks=7&file_name=testing.cpp&file_size=5431&block_size=800
string FileInfo::getUrlPath() {
    ostringstream stream;
    stream << "get_file?ndn_name=" + getNdnName().toUri();
    stream << "&num_blocks=" << numSegs;
    stream << "&file_name=" << filename;
    stream << "&file_size=" << size;
    stream << "&block_size=" << blockSize;
    return stream.str();
}

string FileInfo::toString() {
    ostringstream stream;
    stream << relativePath << ", " << filename << ", " << prefix << ", " << blockSize << ", " << size << ", " << numSegs << ", " << modificationTime;
    return stream.str();
}


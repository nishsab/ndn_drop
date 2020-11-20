//
// Created by NIshant Sabharwal on 11/12/20.
//

#include <iostream>
#include <sstream>
#include "FileInfo.h"
#include "../Utils.h"

FileInfo::FileInfo() {
    cout << "EMPTY!!" << endl;
    //Utils::errf("FileInfo::FileInfo() is not implemented!\n");
    //throw 1;
};

FileInfo::FileInfo(string filename, string relativePath, string prefix, int blockSize) {
    cout << "BUILDT" << endl;
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


//
// Created by NIshant Sabharwal on 11/12/20.
//

#ifndef NDN_DROP_FILEINFO_H
#define NDN_DROP_FILEINFO_H

#include <string>
#include <ndn-cxx/name.hpp>

using namespace std;
using namespace ndn;

class FileInfo {
public:
    FileInfo(string filename, string relativePath, string prefix, int blockSize);
    FileInfo();
    Name getNdnName();
    string toString();

    string filename;
    string relativePath;
    string prefix;
    int blockSize;
    long modificationTime = 0;
    int size = 0;
    int numSegs = 0;
};

#endif //NDN_DROP_FILEINFO_H

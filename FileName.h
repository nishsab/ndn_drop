//
// Created by NIshant Sabharwal on 10/22/20.
//

#ifndef NDN_DROP_FILENAME_H
#define NDN_DROP_FILENAME_H

#include <string>
#include <vector>
#include <ndn-cxx/name.hpp>

using namespace std;
using namespace ndn;

enum FileNameParts
{
    NDN = 0,
    DROP,
    HOME,
    OWNER,
    TYPE,
    FILE_NAME_OR_MAX,
    MAX
};

enum Type
{
    FILE_LIST = 0,
    FILE_NAME = 1
};

// "ndn/drop/nishant/laptop/file_list"
class FileName {
public:
    FileName(string name);
    FileName(string home, string owner);
    Name getFileNamePrefix();
    Name getFileListName();
    Name getFileName(string owner, string filename);
    bool isFileList();
private:
    Type type;
    string filename;
    string home;
    string owner;
};
#endif //NDN_DROP_FILENAME_H

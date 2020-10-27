//
// Created by NIshant Sabharwal on 10/22/20.
//

#include "FileName.h"
#include "Utils.h"

const string file_list = "file_list";
const string file_name = "file_name";


FileName::FileName(string name) {
    vector<string> tokens = Utils::split(name.substr(1), '/');
    if (tokens.size() < FILE_NAME_OR_MAX) {
        throw -1;
    }
    if (tokens[TYPE] == file_list) {
        type = FILE_LIST;
    }
    else if (tokens[TYPE] == file_name) {
        type = FILE_NAME;
        filename = tokens[FILE_NAME_OR_MAX];
    }
    else {
        throw -1;
    }
}

FileName::FileName(string home, string owner) {
    this->home = home;
    this->owner = owner;
}

Name FileName::getFileNamePrefix() {
    Name name("ndn/drop");
    name.append(home);
    name.append(this->owner);
    return name;
}

Name FileName::getFileListName() {
    Name name = getFileNamePrefix();
    name.append(file_list);
    return name;
}

Name FileName::getFileName(string owner, string filename) {
    Name name = getFileNamePrefix();
    name.append(file_name);
    name.append(filename);
    return name;
}

bool FileName::isFileList() {
    return type == FILE_LIST;
}
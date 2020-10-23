//
// Created by NIshant Sabharwal on 10/22/20.
//
#include "DirectoryCrawler.h"
#include <dirent.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include<vector>
#include <iostream>

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;
using namespace std;

string DirectoryCrawler::getFiles() {
    ptree root;
    ptree filesList;
    bool empty = true;


    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (directoryPath.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
                ptree fileTree;
                fileTree.put("filename", ent->d_name);
                filesList.push_back(make_pair("", fileTree));
                empty = false;
            }
        }
        closedir (dir);
    } else {
        /* could not open directory */
        throw -1;
    }
    if (!empty) {
        root.add_child("files", filesList);
        ostringstream buf;
        write_json(buf, root);
        return buf.str();
    }
    else {
        return "{\"files\":[]}";
    }
}

DirectoryCrawler::DirectoryCrawler(string directoryPath) {
    this->directoryPath = directoryPath;
}
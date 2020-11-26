//
// Created by NIshant Sabharwal on 11/14/20.
//

#include "MetadataConverter.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

string MetadataConverter::buildJson(vector<FileInfo> fileInfos) {
    if (fileInfos.empty()) {
        return "{\"files\":[], \"status\": \"success\"}";
    }
    ptree root;
    ptree filesList;

    for (FileInfo fileInfo : fileInfos) {
        ptree fileTree;
        fileTree.put("filename", fileInfo.filename);
        fileTree.put("ndn_name", fileInfo.getNdnName());
        fileTree.put("modification_time", fileInfo.modificationTime);
        fileTree.put("size", fileInfo.size);
        fileTree.put("num_segs", fileInfo.numSegs);
        fileTree.put("block_size", fileInfo.blockSize);
        fileTree.put("url_path", fileInfo.getUrlPath());
        filesList.push_back(make_pair("", fileTree));
    }
    root.add_child("files", filesList);
    root.put("status", "success");
    ostringstream buf;
    write_json(buf, root);
    return buf.str();
}

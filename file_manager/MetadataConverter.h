//
// Created by NIshant Sabharwal on 11/14/20.
//

#ifndef NDN_DROP_METADATACONVERTER_H
#define NDN_DROP_METADATACONVERTER_H

#include <string>
#include "FileInfo.h"

using namespace std;

class MetadataConverter {
public:
    static string buildJson(vector<FileInfo> fileInfos);
};


#endif //NDN_DROP_METADATACONVERTER_H

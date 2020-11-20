//
// Created by NIshant Sabharwal on 11/12/20.
//

#ifndef NDN_DROP_PACKETENCODER_H
#define NDN_DROP_PACKETENCODER_H

#include <ndn-cxx/data.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include "FileInfo.h"
#include "../SecurityPackage.h"

using namespace ndn;

class PacketEncoder {
public:
    PacketEncoder(string certificateName, SecurityPackage *securityPackage);
    vector<const Data> encodePackets(vector<FileInfo> fileInfos);
private:
    KeyChain keyChain;
    vector<const Data> encodeFileIntoPackets(FileInfo fileInfo);
    SecurityPackage *securityPackage;
    string certificateName;
};


#endif //NDN_DROP_PACKETENCODER_H

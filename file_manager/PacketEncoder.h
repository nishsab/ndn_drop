//
// Created by NIshant Sabharwal on 11/12/20.
//

#ifndef NDN_DROP_PACKETENCODER_H
#define NDN_DROP_PACKETENCODER_H

#include <ndn-cxx/data.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include "FileInfo.h"
#include <ndn-cxx/security/validator-config.hpp>
#include <ndn-nac/encryptor.hpp>
#include <ndn-nac/access-manager.hpp>

using namespace ndn;
using namespace nac;

class PacketEncoder {
public:
    PacketEncoder(string certificateName, string pibLocator, string tpmLocator, string nacAccessPrefix,
                  string nacCkPrefix, string schemaConfPath);

    vector<const Data> encodePackets(vector<FileInfo> fileInfos);
private:
    KeyChain keyChain;
    boost::asio::io_service m_ioService;
    Face face;
    ValidatorConfig m_validator;
    Encryptor m_encryptor;
    vector<const Data> encodeFileIntoPackets(FileInfo fileInfo);
    string certificateName;
    void runEncryptor();
    thread m_thread;
};


#endif //NDN_DROP_PACKETENCODER_H

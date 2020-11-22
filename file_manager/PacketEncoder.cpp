//
// Created by NIshant Sabharwal on 11/12/20.
//

#include <fstream>
#include <ndn-cxx/security/signing-helpers.hpp>
#include <iostream>
#include "PacketEncoder.h"
#include "../Utils.h"

PacketEncoder::PacketEncoder(string certificateName,
                             string pibLocator,
                             string tpmLocator,
                             string nacAccessPrefix,
                             string nacCkPrefix,
                             string schemaConfPath)
: keyChain(pibLocator, tpmLocator),
  face(m_ioService),
  m_validator(face),
  m_encryptor(nacAccessPrefix,
              nacCkPrefix, signingWithSha256(),
                      [] (const ErrorCode&, const std::string& error) {
                          cerr << "Failed to publish CK from PacketEncoder: " << error << endl;
                          throw(-1);
                      }, m_validator, keyChain, face)
{
    cout << "1" << endl;
    m_validator.load(schemaConfPath);
    this->certificateName = certificateName;
    //m_ioService.run();
    m_thread = thread(&PacketEncoder::runEncryptor, this);
    cout << "2" << endl;
}

void PacketEncoder::runEncryptor() {
    face.processEvents();
}

vector<const Data> PacketEncoder::encodeFileIntoPackets(FileInfo fileInfo) {
    vector<const Data> dataPackets;
    ifstream infile(fileInfo.relativePath, ofstream::binary);

    uint8_t buffer[fileInfo.blockSize];
    for (int i=0; i<fileInfo.numSegs; i++) {
        Data dataPacket;
        dataPacket.setName(fileInfo.getNdnName().appendSegment(i));
        infile.read((char *) buffer, fileInfo.blockSize);
        //dataPacket.setContent(buffer, infile.gcount());

        auto blob = m_encryptor.encrypt(buffer, infile.gcount());
        dataPacket.setContent(blob.wireEncode());

        dataPacket.setFinalBlock(name::Component::fromSegment(fileInfo.numSegs-1));
        dataPacket.setFreshnessPeriod(0_s);
        keyChain.sign(dataPacket, security::signingByCertificate(Name(certificateName)));

        Utils::logf("PacketEncoder::encodeFileIntoPackets: encoded data packet %s\n", dataPacket.getName().toUri().c_str());
        dataPackets.push_back(dataPacket);
    }
    return dataPackets;
}

vector<const Data> PacketEncoder::encodePackets(vector<FileInfo> fileInfos) {
    vector<const Data> dataPackets;
    for (FileInfo fileInfo : fileInfos) {
        vector<const Data> fileDataPackets = encodeFileIntoPackets(fileInfo);

        for (const Data dataPacket : fileDataPackets) {
            dataPackets.push_back(dataPacket);
        }
    }
    return dataPackets;
}
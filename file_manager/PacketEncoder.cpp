//
// Created by NIshant Sabharwal on 11/12/20.
//

#include <fstream>
#include <ndn-cxx/security/signing-helpers.hpp>
#include "PacketEncoder.h"
#include "../Utils.h"

PacketEncoder::PacketEncoder(string certificateName, SecurityPackage *securityPackage) {
    this->certificateName = certificateName;
    this->securityPackage = securityPackage;
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

        auto blob = securityPackage->m_encryptor.encrypt(buffer, infile.gcount());
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
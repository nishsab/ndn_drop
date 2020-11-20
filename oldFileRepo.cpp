//
// Created by NIshant Sabharwal on 10/22/20.
//

#include <ndn-cxx/security/signing-helpers.hpp>
#include <ndn-cxx/security/validator.hpp>
#include <fstream>
#include <iostream>
#include "FileRepo.h"
#include "FileName.h"
#include "Utils.h"

void FileRepo::setInterestFilter() {
    FileName filename = FileName(this->homeName, this->nodeName);
    Name fileNamePrefix = filename.getFileListName();
    Utils::logf("Listening for file requests on %s\n", fileNamePrefix.toUri().c_str());
    registeredPrefix = face.setInterestFilter(
            fileNamePrefix,
            //bind(&FileRepo::onInterest, this, _2),
            bind(&FileRepo::onInterest, this, _2),
            [] (const auto&, const auto& reason) {
                NDN_THROW(std::runtime_error("Failed to register prefix: " + reason));
            });
    face.processEvents();
}

void FileRepo::onValidInterest(const Interest& interest) {
    Utils::logf("Received an interest: %s\n", interest.getName().toUri().c_str());
    m_validator.validate(interest,
                         [=] (const Interest& data) {
                            cout << "VALIDATED!!!" << endl;
                         },
                         [=] (const Interest& data, const ValidationError& error) {
                             std::cerr << "Cannot validate retrieved data: " << error << std::endl;
                         });
    /*m_validator.validate(interest,
                                          bind(&FileRepo::onInterest, this, _1),
                                          bind(&FileRepo::onInvalidInterest, this, _1, _2));*/
    sleep(2);
    cout << "all done!" << endl;
}

void FileRepo::onInvalidInterest(const Interest& interest, const ValidationError& error) {
    std::cerr << "Cannot validate retrieved data: " << error << std::endl;
    auto data = make_shared<Data>(interest.getName());
    data->setFreshnessPeriod(1_s);
    string response = "{\"status\": \"error\", \"reason\": \"Could not validate.\"}";
    data->setContent((uint8_t *) response.c_str(), response.size());

    keyChain.sign(*data, security::signingByCertificate(Name(this->homeCertificateName)));
    face.put(*data);
}

void FileRepo::onInterest(const Interest& interest)
{
    Utils::logf("Received an interest: %s\n", interest.getName().toUri().c_str());
    try {
        FileName fileName = FileName(interest.getName().toUri());

        if (fileName.isFileList()) {
            Utils::logf("Responding with file list\n");

            string fileMetadataJson = directoryManager->getFileMetaDataJson();

            cout << interest.getSignatureInfo()->getNonce()->data() << endl;
            cout << interest.getSignatureInfo()->getTime()->time_since_epoch() << endl;
            cout << interest.getSignatureInfo()->getSeqNum().value() << endl;
            /*ifstream file(fileListLocation, std::ios::binary | std::ios::ate);
            streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            vector<uint8_t> buffer(size);
            file.read((char *) buffer.data(), size);*/
            auto data = make_shared<Data>(interest.getName());
            data->setFreshnessPeriod(1_s);
            //data->setContent(buffer.data(), static_cast<size_t>(buffer.size()));
            data->setContent((uint8_t *) fileMetadataJson.c_str(), fileMetadataJson.size());

            keyChain.sign(*data, security::signingByCertificate(Name(this->homeCertificateName)));
            face.put(*data);
        }
    }
    catch (int e) {
        Utils::errf("Error: invalid file name: %s\n", interest.getName().toUri().c_str());
    }
}

void FileRepo::stop() {
    m_thread.join();
}

FileRepo::FileRepo(Face &face, string homeName, string nodeName, DirectoryManager *directoryManager, string fileListLocation, string homeCertificateName, Conf *conf)
: face(m_ioService),
  m_validator(face)
{
    //m_ioService.run();
    this->homeName = homeName;
    this->nodeName = nodeName;
    this->directoryManager = directoryManager;
    this->fileListLocation = fileListLocation;
    m_thread = thread(&FileRepo::setInterestFilter, this);
    this->homeCertificateName = homeCertificateName;
    cout << conf->schemaConfPath << endl;
    m_validator.load(conf->schemaConfPath);
}
//
// Created by NIshant Sabharwal on 10/22/20.
//

#include <ndn-cxx/security/signing-helpers.hpp>
#include <ndn-cxx/security/validator.hpp>
#include <fstream>
#include "FileRepo.h"
#include "FileName.h"
#include "Utils.h"

void FileRepo::setInterestFilter() {
    FileName filename = FileName(this->homeName, this->nodeName);
    Name fileNamePrefix = filename.getFileListName();
    Utils::logf("Listening for file requests on %s\n", fileNamePrefix.toUri().c_str());
    registeredPrefix = face.setInterestFilter(
            fileNamePrefix,
            bind(&FileRepo::onInterest, this, _2),
            [] (const auto&, const auto& reason) {
                NDN_THROW(std::runtime_error("Failed to register prefix: " + reason));
            });
    face.processEvents();
}

void FileRepo::onInterest(const Interest& interest)
{
    Utils::logf("Received an interest: %s\n", interest.getName().toUri().c_str());
    try {
        FileName fileName = FileName(interest.getName().toUri());

        if (fileName.isFileList()) {
            Utils::logf("Responding with file list\n");



            ifstream file(fileListLocation, std::ios::binary | std::ios::ate);
            streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            vector<uint8_t> buffer(size);
            file.read((char *) buffer.data(), size);
            auto data = make_shared<Data>(interest.getName());
            data->setFreshnessPeriod(1_s);
            data->setContent(buffer.data(), static_cast<size_t>(buffer.size()));

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

FileRepo::FileRepo(Face &face, string homeName, string nodeName, DirectoryCrawler *directoryCrawler, string fileListLocation, string homeCertificateName) : face(m_ioService) {
    m_ioService.run();
    this->homeName = homeName;
    this->nodeName = nodeName;
    this->directoryCrawler = directoryCrawler;
    this->fileListLocation = fileListLocation;
    m_thread = thread(&FileRepo::setInterestFilter, this);
    this->homeCertificateName = homeCertificateName;
}
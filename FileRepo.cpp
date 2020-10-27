//
// Created by NIshant Sabharwal on 10/22/20.
//

#include <ndn-cxx/security/signing-helpers.hpp>
#include "FileRepo.h"
#include "FileName.h"
#include "Utils.h"
#include "IdentityName.h"

void FileRepo::setInterestFilter() {
    FileName filename = FileName(this->homeName, this->nodeName);
    Name fileNamePrefix = filename.getFileNamePrefix();
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
            string fileListJson = directoryCrawler->getFiles();

            std::vector<uint8_t> buffer(fileListJson.begin(), fileListJson.end());
            auto data = make_shared<Data>(interest.getName());
            data->setFreshnessPeriod(1_s);
            data->setContent(buffer.data(), static_cast<size_t>(fileListJson.length()));

            keyChain.sign(*data, security::signingByIdentity(IdentityName::getIdentityName(homeName, nodeName)));
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

FileRepo::FileRepo(Face &face, string homeName, string nodeName, DirectoryCrawler *directoryCrawler) : face(m_ioService) {
    m_ioService.run();
    this->homeName = homeName;
    this->nodeName = nodeName;
    this->directoryCrawler = directoryCrawler;

    m_thread = thread(&FileRepo::setInterestFilter, this);
}
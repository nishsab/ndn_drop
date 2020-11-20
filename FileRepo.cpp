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
    RegisteredPrefixHandle registeredPrefix = face.setInterestFilter(
            fileNamePrefix,
            bind(&FileRepo::onInterest, this, _2),
            [] (const auto&, const auto& reason) {
                NDN_THROW(std::runtime_error("Failed to register prefix: " + reason));
            });
    face.processEvents();
}

void FileRepo::onInterest(const Interest& interest) {
    Utils::logf("FileRepo::onInterest: Received an interest: %s\n", interest.getName().toUri().c_str());
    m_validator.validate(interest,
                         bind(&FileRepo::onValidInterest, this, _1),
                         bind(&FileRepo::onInvalidInterest, this, _1, _2));
}

void FileRepo::onInvalidInterest(const Interest& interest, const ValidationError& error) {
    Utils::errf("FileRepo::onInvalidInterest: could not validate interest: %s\n", error.getInfo().c_str());
    auto data = make_shared<Data>(interest.getName());
    data->setFreshnessPeriod(1_s);
    string response = "{\"status\": \"error\", \"reason\": \"Could not validate: " + error.getInfo() + "\"}";
    data->setContent((uint8_t *) response.c_str(), response.size());
    keyChain.sign(*data, security::signingByCertificate(Name(this->homeCertificateName)));
    face.put(*data);
}

void FileRepo::onValidInterest(const Interest& interest)
{
    Utils::logf("FileRepo::onValidInterest: Responding with file list\n");
    grantAccess(interest);
    string fileMetadataJson = directoryManager->getFileMetaDataJson();
    auto data = make_shared<Data>(interest.getName());
    data->setFreshnessPeriod(1_s);

    //data->setContent((uint8_t *) fileMetadataJson.c_str(), fileMetadataJson.size());
    auto blob = m_encryptor.encrypt(reinterpret_cast<const uint8_t*>(fileMetadataJson.data()), fileMetadataJson.size());
    data->setContent(blob.wireEncode());

    keyChain.sign(*data, security::signingByCertificate(Name(this->homeCertificateName)));
    face.put(*data);
}

void FileRepo::grantAccess(const Interest& interest) {
    if (interest.getSignatureInfo().has_value()) {
        Name signingKeyName = interest.getSignatureInfo()->getKeyLocator().getName();
        if (m_validator.getUnverifiedCertCache().find(signingKeyName)) {
            const Certificate *signingCertificate = m_validator.getUnverifiedCertCache().find(signingKeyName);
            Utils::logf("FileRepo::grantAccess: granting access to %s from unverified cache\n", signingCertificate->getKeyName().toUri().c_str());
            m_accessManager.addMember(*signingCertificate);
        }
        else if (m_validator.getVerifiedCertCache().find(signingKeyName)) {
            const Certificate *signingCertificate = m_validator.getVerifiedCertCache().find(signingKeyName);
            Utils::logf("FileRepo::grantAccess: granting access to %s from verified cache\n", signingCertificate->getKeyName().toUri().c_str());
            m_accessManager.addMember(*signingCertificate);
        }
        else {
            Utils::errf("FileRepo::grantAccess: Certificate (%s) not found so no access granted.\n", signingKeyName.toUri().c_str());
        }
    }
    else{
        Utils::errf("FileRepo::grantAccess: No signing info so no access granted.\n");
    }
}

void FileRepo::stop() {
    m_thread.join();
}

FileRepo::FileRepo(string pibLocator,
                   string tpmLocator,
                   string homeName,
                   string nodeName,
                   DirectoryManager *directoryManager,
                   string homeCertificateName,
                   string schemaConfPath,
                   string nacIdentityName,
                   string nacDataName,
                   string nacAccessPrefix,
                   string nacCkPrefix)
: keyChain(pibLocator, tpmLocator),
  face(m_ioService),
  m_validator(face),
  m_accessManager(keyChain.getPib().getIdentity(Name(nacIdentityName)), nacDataName,
                  keyChain, face),
  m_encryptor(nacAccessPrefix,
              nacCkPrefix, signingWithSha256(),
              [] (auto...) {
                  cerr << "Failed to publish CK";
              }, m_validator, keyChain, face)
{
    m_validator.load(schemaConfPath);
    this->homeName = homeName;
    this->nodeName = nodeName;
    this->directoryManager = directoryManager;
    m_thread = thread(&FileRepo::setInterestFilter, this);
    this->homeCertificateName = homeCertificateName;
}
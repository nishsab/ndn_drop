//
// Created by NIshant Sabharwal on 10/25/20.
//

#include <iostream>
#include <ndn-cxx/security/signing-helpers.hpp>
#include "FileRequestor.h"
#include "Utils.h"
#include "FileName.h"
#include <ndn-cxx/util/random.hpp>

using namespace std;

FileRequestor::FileRequestor(string homeName,
                             string schemaConfPath,
                             string homeCertificateName,
                             string pibLocator,
                             string tpmLocator)
: keyChain(pibLocator, tpmLocator),
  m_face(m_ioService),
  m_validator(m_face),
  m_decryptor(keyChain.getPib().getIdentity(Name(homeCertificateName).getPrefix(-4)).getKey(Name(homeCertificateName).getPrefix(-2)),
            m_validator, keyChain, m_face)
{
    m_validator.load(schemaConfPath);
    this->homeName = homeName;
    this->homeCertificateName = homeCertificateName;
}

string FileRequestor::getFileList(string owner) {
    CallbackBlocker blocker;
    FileName fileName = FileName(homeName, owner);
    Name fileListname = fileName.getFileListName();
    Utils::logf("FileRequestor::getFileList: Sending request for %s\n", fileListname.toUri().c_str());
    Interest interest(fileListname);

    ndn::security::SigningInfo signingInfo = security::signingByCertificate(Name(this->homeCertificateName));
    SignatureInfo info = signingInfo.getSignatureInfo();
    std::vector<uint8_t> nonce(8);
    random::generateSecureBytes(nonce.data(), nonce.size());
    info.setNonce(nonce);
    info.setTime();
    info.setSeqNum(0);
    signingInfo.setSignatureInfo(info);
    signingInfo.setSignedInterestFormat(ndn::security::SignedInterestFormat::V03);
    keyChain.sign(interest, signingInfo);

    m_face.expressInterest(interest,
                           bind(&FileRequestor::handleFileListResponse, this,  _1, _2, &blocker),
                           bind(&FileRequestor::onNack, this, &blocker),
                           bind(&FileRequestor::onTimeout, this, &blocker));

    m_face.processEvents();
    return blocker.val;
}

void FileRequestor::handleFileListResponse(const Interest&, const Data& data, CallbackBlocker *callbackBlocker)
{
    m_validator.validate(data,
                         bind(&FileRequestor::unpackData, this, _1, callbackBlocker),
                         bind(&FileRequestor::validationError, this, _2, callbackBlocker));
}

void FileRequestor::unpackData(const Data& data, CallbackBlocker *callbackBlocker)
{
    Utils::logf("FileRequestor::unpackData: validation succeeded.\n");
    m_decryptor.decrypt(data.getContent().blockFromValue(),
                        [=] (ConstBufferPtr content) {
                            string decryptedContent = std::string(reinterpret_cast<const char*>(content->data()), content->size());
                            Utils::logf("FileRequestor::unpackData: successfully decrypted content.\n");
                            callbackBlocker->val = decryptedContent;
                        },
                        [=] (const ErrorCode&, const std::string& error) {
                            Utils::errf("FileRequestor::unpackData: Received a validation error: %s.\n", error.c_str());
                            callbackBlocker->val = "{\"status\": \"error\", \"reason\": \"Could not decrypt: " + error + ".\"}";
                        });
    Utils::logf("FileRequestor::unpackData: done decrypting.\n");
}

void FileRequestor::validationError(const ndn::security::ValidationError &error, CallbackBlocker *callbackBlocker) {
    Utils::errf("FileRequestor::validationError: Received a validation error.\n");
    ostringstream stream;
    stream << "{\"status\": \"error\", \"reason\": \"" << error.getInfo() << "\"}";
    callbackBlocker->val = stream.str();
}

void FileRequestor::onNack(CallbackBlocker *callbackBlocker)
{
    Utils::errf("FileRequestor::onNack: Received a nack.\n");
    callbackBlocker->val = "{\"status\": \"error\", \"reason\": \"Received a nack.\"}";
}

void FileRequestor::onTimeout(CallbackBlocker *callbackBlocker)
{
    Utils::errf("FileRequestor::onTimeout: Received a timeout.\n");
    callbackBlocker->val = "{\"status\": \"error\", \"reason\": \"Received a timeout.\"}";
}
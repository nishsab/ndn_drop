//
// Created by NIshant Sabharwal on 11/10/20.
//

#include <ndn-cxx/security/signing-helpers.hpp>
#include <ndn-cxx/security/interest-signer.hpp>
#include <iostream>
#include "KeyRequestor.h"
#include "Utils.h"
#include <ndn-cxx/util/random.hpp>

KeyRequestor::KeyRequestor(string homeName, string nodeName, string schemaConfPath, string homeCertificateName)
: m_validator(m_face),
m_decryptor(keyChain.getPib().getIdentity(Name(homeCertificateName).getPrefix(-4)).getKey(Name(homeCertificateName).getPrefix(-2)), m_validator, keyChain, m_face)
{
    m_validator.load(schemaConfPath);
    this->homeName = homeName;
    this->homeCertificateName = homeCertificateName;
}

string KeyRequestor::getFileList(string owner) {
    CallbackBlocker blocker;
    Name name = Name(owner);
    name.appendTimestamp();
    cout << name << endl;
    Utils::logf("FileRequestor::getFileList: Sending request for %s\n", name.toUri().c_str());
    Interest interest(name);
    interest.setCanBePrefix(true);
    interest.setMustBeFresh(true);

    ndn::security::SigningInfo signingInfo = security::signingByCertificate(Name(this->homeCertificateName));
    /*signingInfo.setSignedInterestFormat(ndn::security::SignedInterestFormat::V03);
    keyChain.sign(interest, signingInfo);
    cout << interest.getName() << endl;
    cout << interest.getSignatureInfo().has_value() << endl;*/

    SignatureInfo info = signingInfo.getSignatureInfo();
    std::vector<uint8_t> nonce(8);
    random::generateSecureBytes(nonce.data(), nonce.size());
    info.setNonce(nonce);
    info.setTime();
    info.setSeqNum(0);
    signingInfo.setSignatureInfo(info);
    signingInfo.setSignedInterestFormat(ndn::security::SignedInterestFormat::V03);
    keyChain.sign(interest, signingInfo);


            //keyChain.sign(interest, security::signingByCertificate(Name(this->homeCertificateName)));
    //keyChain.sign(interest, security::signingByIdentity(keyChain.getPib().getDefaultIdentity()));
    //Block block = interest.wireEncode();
    //keyChain.sign(block.getBuffer()->get<uint8_t>(), block.size(), security::signingByCertificate(Name(this->homeCertificateName)));
    //keyChain.sign(block, block.size(), security::signingByCertificate(Name(this->homeCertificateName)));
    m_face.expressInterest(interest,
                           bind(&KeyRequestor::handleFileListResponse, this,  _1, _2, &blocker),
                           bind(&KeyRequestor::onNack, this, &blocker),
                           bind(&KeyRequestor::onTimeout, this, &blocker));
    m_face.processEvents();
    return blocker.val;
}

void KeyRequestor::handleFileListResponse(const Interest&, const Data& data, CallbackBlocker *callbackBlocker)
{
    const Block& content = data.getContent();
    string val = string(content.value(), content.value() + content.value_size());

    m_decryptor.decrypt(data.getContent().blockFromValue(),
                        [=] (ConstBufferPtr content) {
        cout << "SUCCESS!" << endl;
                            std::cout << "Decrypted content: "
                                      << std::string(reinterpret_cast<const char*>(content->data()), content->size())
                                      << std::endl;
                        },
                        [=] (const ErrorCode&, const std::string& error) {
        cout << "FAIL!" << endl;
                            std::cerr << "Cannot decrypt data: " << error << std::endl;
                        });

    Utils::logf("FileRequestor::handleFileListResponse: %s\n", val.c_str());
    callbackBlocker->val = val;
}

void KeyRequestor::onNack(CallbackBlocker *callbackBlocker)
{
    Utils::errf("FileRequestor::onNack: Received a nack.\n");
    callbackBlocker->val = "{\"status\": \"error\", \"reason\": \"Received a nack.\"}";
}

void KeyRequestor::onTimeout(CallbackBlocker *callbackBlocker)
{
    Utils::errf("FileRequestor::onNack: Received a nack.\n");
    callbackBlocker->val = "{\"status\": \"error\", \"reason\": \"Received a nack.\"}";
}
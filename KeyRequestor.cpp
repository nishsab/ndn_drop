//
// Created by NIshant Sabharwal on 11/10/20.
//

#include <ndn-cxx/security/signing-helpers.hpp>
#include <ndn-cxx/security/interest-signer.hpp>
#include <iostream>
#include "KeyRequestor.h"
#include "Utils.h"
#include "SessionKeyName.h"
#include <ndn-cxx/util/random.hpp>

KeyRequestor::KeyRequestor(string schemaConfPath, string homeCertificateName)
: m_validator(m_face),
m_decryptor(keyChain.getPib().getIdentity(Name(homeCertificateName).getPrefix(-4)).getKey(Name(homeCertificateName).getPrefix(-2)), m_validator, keyChain, m_face)
{
    m_validator.load(schemaConfPath);
    this->homeCertificateName = homeCertificateName;
}

bool KeyRequestor::requestAccess(string home, string owner) {
    CallbackBlocker blocker;
    Name sessionKeyName = SessionKeyName::getSessionKeyName(home, owner, "");
    /*Name name = Name(owner);
    name.appendTimestamp();
    cout << name << endl;*/
    Utils::logf("KeyRquestor::requestAccess: Sending request for %s\n", sessionKeyName.toUri().c_str());
    Interest interest(sessionKeyName);
    interest.setCanBePrefix(true);
    interest.setMustBeFresh(true);
    cout << "a" << endl;
    ndn::security::SigningInfo signingInfo = security::signingByCertificate(Name(this->homeCertificateName));
    cout << "b" << endl;
    SignatureInfo info = signingInfo.getSignatureInfo();
    std::vector<uint8_t> nonce(8);
    random::generateSecureBytes(nonce.data(), nonce.size());
    info.setNonce(nonce);
    info.setTime();
    info.setSeqNum(0);
    signingInfo.setSignatureInfo(info);
    signingInfo.setSignedInterestFormat(ndn::security::SignedInterestFormat::V03);
    keyChain.sign(interest, signingInfo);
    cout << "c" << endl;

    m_face.expressInterest(interest,
                           bind(&KeyRequestor::handleFileListResponse, this,  _1, _2, &blocker),
                           bind(&KeyRequestor::onNack, this, &blocker),
                           bind(&KeyRequestor::onTimeout, this, &blocker));
    m_face.processEvents();
    cout << "d" << endl;
    cout << blocker.val << endl;
    return blocker.val.find("success") != string::npos;
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
    interest.setInterestLifetime(5_s);
    m_face.expressInterest(interest,
                           bind(&KeyRequestor::handleFileListResponse, this,  _1, _2, &blocker),
                           bind(&KeyRequestor::onNack, this, &blocker),
                           bind(&KeyRequestor::onTimeout, this, &blocker));
    m_face.processEvents();
    return blocker.val;
}

void KeyRequestor::handleFileListResponse(const Interest&, const Data& data, CallbackBlocker *callbackBlocker)
{
    /*cout << "about to delete" << endl;
    try {
        Identity id = keyChain.getPib().getIdentity(Name("/ndn/drop/nishant/laptop/access"));
        cout << "identity: " << id << endl;
        cout << id.getKeys().size() << endl;
        if (id.getKeys().size() > 0) {
            Key key = keyChain.getPib().getIdentity(Name("/ndn/drop/nishant/laptop/access")).getDefaultKey();
            cout << "key: " << key << endl;
            keyChain.deleteKey(id, key);
        }
    } catch(int e) {
            cerr << "exception: " << e << endl;
            exit(1);
        }
    cout << "deleted" << endl;*/
    EncryptedContent encryptedContent(data.getContent().blockFromValue());
    cout << encryptedContent.getKeyLocator().toUri() << endl;
    m_decryptor.decrypt(data.getContent().blockFromValue(),
                        [=] (ConstBufferPtr content) {
                            cout << "SUCCESS!" << endl;
                            cout << "SUCCESS!" << endl;
                            cout << "SUCCESS!" << endl;
                            cout << "SUCCESS!" << endl;
                            cout << "SUCCESS!" << endl;
                            cout << "SUCCESS!" << endl;
                            cout << "SUCCESS!" << endl;
                            string decrypted = std::string(reinterpret_cast<const char*>(content->data()), content->size());
                            std::cout << "Decrypted content: "
                                      << decrypted
                                      << std::endl;
                        },
                        [=] (const ErrorCode&, const std::string& error) {
                            cout << "FAIL!" << endl;
                            cout << "FAIL!" << endl;
                            cout << "FAIL!" << endl;
                            cout << "FAIL!" << endl;
                            cout << "FAIL!" << endl;
                            cout << "FAIL!" << endl;
                            cout << "FAIL!" << endl;
                            cout << "FAIL!" << endl;
                            cout << "FAIL!" << endl;
                            std::cerr << "Cannot decrypt data: " << error << std::endl;
                        });

    sleep(2);
    const Block& content = data.getContent();
    string val = string(content.value(), content.value() + content.value_size());


    Utils::logf("KeyRequestor::handleFileListResponse: received %s\n", val.c_str());
    callbackBlocker->val = val;
}

void KeyRequestor::onNack(CallbackBlocker *callbackBlocker)
{
    Utils::errf("KeyRequestor::onNack: Received a nack.\n");
    callbackBlocker->val = "{\"status\": \"error\", \"reason\": \"Received a nack.\"}";
}

void KeyRequestor::onTimeout(CallbackBlocker *callbackBlocker)
{
    Utils::errf("KeyRequestor::onTimeout: Received a timeout.\n");
    callbackBlocker->val = "{\"status\": \"error\", \"reason\": \"Received a timeout.\"}";
}
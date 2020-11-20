//
// Created by NIshant Sabharwal on 11/10/20.
//

#ifndef NDN_DROP_KEYREQUESTOR_H
#define NDN_DROP_KEYREQUESTOR_H

#include <string>
#include <boost/asio/io_service.hpp>
#include <ndn-cxx/security/validator-config.hpp>
#include <ndn-cxx/face.hpp>
#include "FileRequestor.h"
#include <ndn-nac/decryptor.hpp>

using namespace std;
using namespace ndn;
using namespace nac;

class KeyRequestor {
public:
    KeyRequestor(string schemaConfPath, string homeCertificateName);
    string getFileList(string owner);
    bool requestAccess(string home, string owner);

private:
    void handleFileListResponse(const Interest&, const Data& data, CallbackBlocker *callbackBlocker);
    void unpackData(const Data& data, CallbackBlocker *callbackBlocker);
    void validationError(const ndn::security::ValidationError& error, CallbackBlocker *callbackBlocker);
    void onNack(CallbackBlocker *blocker);
    void onTimeout(CallbackBlocker *callbackBlocker);
    KeyChain keyChain;
    Face m_face;
    ValidatorConfig m_validator;
    string homeCertificateName;
    Decryptor m_decryptor;
};


#endif //NDN_DROP_KEYREQUESTOR_H

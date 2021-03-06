//
// Created by NIshant Sabharwal on 10/25/20.
//

#ifndef NDN_DROP_FILEREQUESTOR_H
#define NDN_DROP_FILEREQUESTOR_H

#include <ndn-cxx/face.hpp>
#include "NeighborList.h"
#include <boost/asio/io_service.hpp>
#include <ndn-cxx/security/validator-config.hpp>
#include <ndn-nac/decryptor.hpp>

using namespace ndn;
using namespace nac;

struct CallbackBlocker {
    string val;
};

class FileRequestor {
public:
    FileRequestor(string homeName,
                  string schemaConfPath,
                  string homeCertificateName,
                  string pibLocator,
                  string tpmLocator);
    string getFileList(string owner);

private:
    void handleFileListResponse(const Interest&, const Data& data, CallbackBlocker *callbackBlocker);
    void unpackData(const Data& data, CallbackBlocker *callbackBlocker);
    void validationError(const ndn::security::ValidationError& error, CallbackBlocker *callbackBlocker);
    void onNack(CallbackBlocker *blocker);
    void onTimeout(CallbackBlocker *callbackBlocker);
    string homeName;
    boost::asio::io_service m_ioService;
    KeyChain keyChain;
    Face m_face;
    ValidatorConfig m_validator;
    string homeCertificateName;
    Decryptor m_decryptor;
};

#endif //NDN_DROP_FILEREQUESTOR_H

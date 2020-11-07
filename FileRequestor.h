//
// Created by NIshant Sabharwal on 10/25/20.
//

#ifndef NDN_DROP_FILEREQUESTOR_H
#define NDN_DROP_FILEREQUESTOR_H

#include <ndn-cxx/face.hpp>
#include "NeighborList.h"
#include <boost/asio/io_service.hpp>
#include <ndn-cxx/security/validator-config.hpp>

struct CallbackBlocker {
    string val;
};

class FileRequestor {
public:
    FileRequestor(string homeName, string nodeName, string schemaConfPath);
    string getFileList(string owner);

private:
    void handleFileListResponse(const Interest&, const Data& data, CallbackBlocker *callbackBlocker);
    void unpackData(const Data& data, CallbackBlocker *callbackBlocker);
    void validationError(const ndn::security::ValidationError& error, CallbackBlocker *callbackBlocker);
    void onNack(CallbackBlocker *blocker);
    void onTimeout(CallbackBlocker *callbackBlocker);
    string homeName;
    Face m_face;
    ValidatorConfig m_validator;
};


#endif //NDN_DROP_FILEREQUESTOR_H

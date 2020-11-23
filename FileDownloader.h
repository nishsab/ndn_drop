//
// Created by NIshant Sabharwal on 11/4/20.
//

#ifndef NDN_DROP_FILEDOWNLOADER_H
#define NDN_DROP_FILEDOWNLOADER_H

#include <string>
#include <ndn-cxx/face.hpp>
#include "NeighborList.h"
#include <boost/asio/io_service.hpp>
#include <ndn-cxx/security/validation-error.hpp>
#include <ndn-nac/access-manager.hpp>
#include <ndn-nac/decryptor.hpp>
#include <ndn-cxx/security/validator-config.hpp>

using namespace std;
using namespace ndn;
using namespace nac;

struct CallbackContainer {
    ofstream *stream;
    mutex lock;
    int counter;
    bool error;
    string errorMessage;
};

class FileDownloader {
public:
    FileDownloader(string inboundDirectoryPath,
                   string homeCertificateName,
                   string schemaConfPath,
                   string pibLocator,
                   string tpmLocator);
    string getFile(string ndnName, int numBlocks, string filename, int fileSize, int blockSize, string owner);

private:
    void handleFileResponse(const Interest&, const Data& data, CallbackContainer *callbackContainer, int blockid, int blockSize);
    void afterValidation(const Data& data, CallbackContainer *callbackContainer, int blockid, int blockSize);
    void validationError(const ndn::security::ValidationError &error, CallbackContainer *callbackContainer);
    void onNack();
    void onTimeout();

    KeyChain keyChain;
    boost::asio::io_service m_ioService;
    Face m_face;
    ValidatorConfig m_validator;
    Decryptor m_decryptor;

    string inboundDirectoryPath;
    string schemaConfPath;
};


#endif //NDN_DROP_FILEDOWNLOADER_H

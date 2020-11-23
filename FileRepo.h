//
// Created by NIshant Sabharwal on 10/22/20.
//

#ifndef NDN_DROP_FILEREPO_H
#define NDN_DROP_FILEREPO_H

#include <ndn-cxx/face.hpp>
#include <boost/asio/io_service.hpp>
#include "file_manager/DirectoryManager.h"
#include "Conf.h"
#include <thread>
#include <ndn-nac/encryptor.hpp>
#include <ndn-nac/access-manager.hpp>


using namespace ndn;
using namespace std;

class FileRepo {
public:
    FileRepo(string pibLocator,
             string tpmLocator,
             string homeName,
             string nodeName,
             DirectoryManager *directoryManager,
             string homeCertificateName,
             string schemaConfPath,
             string nacIdentityName,
             string nacDataName,
             string nacAccessPrefix,
             string nacCkPrefix);
    void stop();
    DirectoryManager *directoryManager;

private:
    void setInterestFilter();
    void grantAccess(const Interest& interest);
    boost::asio::io_service m_ioService;
    KeyChain keyChain;
    Face face;
    string homeName;
    string nodeName;
    void onInterest(const Interest& interest);
    void onValidInterest(const Interest& interest);
    void onInvalidInterest(const Interest& interest, const ValidationError& error);
    thread m_thread;
    string homeCertificateName;
    ValidatorConfig m_validator;
    AccessManager m_accessManager;
    Encryptor m_encryptor;
};


#endif //NDN_DROP_FILEREPO_H

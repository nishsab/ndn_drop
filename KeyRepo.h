//
// Created by NIshant Sabharwal on 11/7/20.
//

#ifndef NDN_DROP_KEYREPO_H
#define NDN_DROP_KEYREPO_H

#include <ndn-cxx/face.hpp>
#include <boost/asio/io_service.hpp>
#include "DirectoryCrawler.h"
#include <thread>
#include <ndn-cxx/security/validator-config.hpp>
#include <ndn-nac/encryptor.hpp>
#include <ndn-nac/access-manager.hpp>

using namespace ndn;
using namespace std;
using namespace nac;

class KeyRepo {
public:
    KeyRepo(string homeName, string nodeName, string homeCertificateName, string schemaConfPath,
            string nacIdentityName, string nacDataName, string nacAccessPrefix, string nacCkPrefix);
    void stop();

private:
    void grantAccessAndRespond(const Interest& interest);
    void reportAccessFailure(const Interest& interest, const ValidationError& error);
    void setInterestFilter();
    RegisteredPrefixHandle registeredPrefix;
    boost::asio::io_service m_ioService;
    KeyChain keyChain;
    Face face;
    string homeName;
    string nodeName;
    string sessionKey;
    void onInterest(const Interest& interest);
    DirectoryCrawler *directoryCrawler;
    thread m_thread;
    string fileListLocation;
    string homeCertificateName;

    ValidatorConfig m_validator;
    AccessManager m_accessManager;
    Encryptor m_encryptor;
};


#endif //NDN_DROP_KEYREPO_H

//
// Created by NIshant Sabharwal on 11/15/20.
//

#ifndef NDN_DROP_SECURITYPACKAGE_H
#define NDN_DROP_SECURITYPACKAGE_H

#include <ndn-cxx/security/validator-config.hpp>
#include <ndn-nac/encryptor.hpp>
#include <ndn-nac/access-manager.hpp>
#include <ndn-nac/decryptor.hpp>

using namespace ndn;
using namespace std;
using namespace nac;


class SecurityPackage {
public:
    SecurityPackage(string pibLocator,
                    string tpmLocator,
                    string homeCertificateName,
                    string schemaConfPath,
                    string nacIdentityName,
                    string nacDataName,
                    string nacAccessPrefix,
                    string nacCkPrefix);
    boost::asio::io_service m_ioService;
    KeyChain keyChain;
    Face face;
    ValidatorConfig m_validator;
    AccessManager m_accessManager;
    Encryptor m_encryptor;
    Decryptor m_decryptor;
};


#endif //NDN_DROP_SECURITYPACKAGE_H

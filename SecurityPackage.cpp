//
// Created by NIshant Sabharwal on 11/15/20.
//

#include <iostream>
#include "SecurityPackage.h"

SecurityPackage::SecurityPackage(string pibLocator,
                                 string tpmLocator,
                                 string homeCertificateName,
                                 string schemaConfPath,
                                 string nacIdentityName,
                                 string nacDataName,
                                 string nacAccessPrefix,
                                 string nacCkPrefix)
: keyChain(pibLocator, tpmLocator),
  face(m_ioService),
  m_validator(face),
  m_accessManager(keyChain.getPib().getIdentity(Name(nacIdentityName)),
                  nacDataName, keyChain, face),
  m_encryptor(nacAccessPrefix,
              nacCkPrefix, signingWithSha256(),
              [] (auto...) {
                  cerr << "Failed to publish CK";
              }, m_validator, keyChain, face),
  m_decryptor(keyChain.getPib().getIdentity(Name(homeCertificateName).getPrefix(-4)).getKey(Name(homeCertificateName).getPrefix(-2)),
              m_validator, keyChain, face)
{
    m_validator.load(schemaConfPath);
}
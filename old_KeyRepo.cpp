//
// Created by NIshant Sabharwal on 11/7/20.
//

#include <iostream>
#include <fstream>
#include "KeyRepo.h"
#include "SessionKeyName.h"
#include "Utils.h"

void KeyRepo::setInterestFilter() {
    cout << "HERE" << endl;
    cout << "HERE" << endl;
    cout << "HERE" << endl;
    Name sessionKeyName = SessionKeyName::getSessionKeyName(this->homeName, this->nodeName, this->sessionKey);
    Utils::logf("Listening for file requests on %s\n", sessionKeyName.toUri().c_str());
    registeredPrefix = face.setInterestFilter(
            sessionKeyName,
            bind(&KeyRepo::onInterest, this, _2),
            [] (const auto&, const auto& reason) {
                NDN_THROW(std::runtime_error("Failed to register prefix: " + reason));
            });
    face.processEvents();
}

void KeyRepo::onInterest(const Interest& interest)
{
    Utils::logf("Received an interest: %s\n", interest.getName().toUri().c_str());
    m_validator.validate(interest,
                         bind(&KeyRepo::grantAccessAndRespond, this, _1),
                         bind(&KeyRepo::reportAccessFailure, this, _1, _2));
}

void KeyRepo::grantAccessAndRespond(const Interest& interest) {
    Name signingKeyName = interest.getSignatureInfo()->getKeyLocator().getName();
    cout << "key signing interest: " << signingKeyName << endl;
    //m_accessManager.addMember(m_keyChain.getPib().getDefaultIdentity().getDefaultKey().getDefaultCertificate());
    const Certificate *signingCertificate = m_validator.getUnverifiedCertCache().find(
        signingKeyName);
    cout << signingCertificate << endl;
    if (signingCertificate) {
        cout << signingCertificate->getName() << endl;
        m_accessManager.addMember(*signingCertificate);
    }
    std::cout << "<< I: " << interest << std::endl;
    Name consumerName = interest.getSignatureInfo()->getKeyLocator().getName().getPrefix(-4);

    // Create new name, based on Interest's name
    Name dataName(interest.getName());
    dataName
            .append("testApp") // add "testApp" component to Interest name
            .appendVersion();  // add "version" component (current UNIX timestamp in milliseconds)

    static const std::string content = "HELLO KITTY";

    // Create Data packet
    shared_ptr<Data> data = make_shared<Data>();
    data->setName(dataName);
    data->setFreshnessPeriod(10_s); // 10 seconds

    auto blob = m_encryptor.encrypt(reinterpret_cast<const uint8_t*>(content.data()), content.size());
    data->setContent(blob.wireEncode());

    // Sign Data packet with default identity
    keyChain.sign(*data, security::signingByCertificate(Name(this->homeCertificateName)));

    // m_keyChain.sign(data, <identityName>);
    // m_keyChain.sign(data, <certificate>);

    // Return Data packet to the requester
    std::cout << ">> D: " << *data << std::endl;

    Block block = data->wireEncode();
    ofstream fout;
    fout.open("file.bin", ios::binary | ios::out);
    cout << block.size() << endl;
    cout << block.getBuffer()->size() << endl;
    fout.write((char *) block.wire(), block.size());
    fout.close();

    face.put(*data);
}

void KeyRepo::reportAccessFailure(const Interest &interest, const ValidationError &error) {
    Utils::errf("FileRequestor::validationError: Received a validation error: %s.\n", error.getInfo().c_str());
    cout << interest.getName() << endl;
    cout << interest.getName().at(command_interest::POS_TIMESTAMP).isNumber() << endl;
    cout << interest.getSignatureInfo()->getKeyLocator().getName() << endl;
    cout << interest.getSignatureInfo()->getTime().value() << endl;
    //auto timestamp = interest.getSignatureInfo()->getTime();
    //cout << *timestamp << endl;
    ostringstream stream;
    stream << "{\"status\": \"error\", \"reason\": \"" << error.getInfo() << "\"}";
    cout << stream.str();
}

KeyRepo::KeyRepo(string homeName, string nodeName, string homeCertificateName, string schemaConfPath,
                 string nacIdentityName, string nacDataName, string nacAccessPrefix, string nacCkPrefix)
: face(m_ioService),
m_validator(face),
m_accessManager(keyChain.getPib().getIdentity(Name(nacIdentityName)), nacDataName,
                         keyChain, face),
m_encryptor(nacAccessPrefix,
                      nacCkPrefix, signingWithSha256(),
                      [] (auto...) {
                          cerr << "Failed to publish CK";
                      }, m_validator, keyChain, face) {
    m_validator.load(schemaConfPath);
    cout << "C" << endl;
    //m_ioService.run();
    cout << "D" << endl;
    this->homeName = homeName;
    this->nodeName = nodeName;
    this->directoryCrawler = directoryCrawler;
    this->fileListLocation = fileListLocation;
    cout << "about to start!!!" << endl;
    m_thread = thread(&KeyRepo::setInterestFilter, this);
    this->homeCertificateName = homeCertificateName;
}
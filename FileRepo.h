//
// Created by NIshant Sabharwal on 10/22/20.
//

#ifndef NDN_DROP_FILEREPO_H
#define NDN_DROP_FILEREPO_H

#include <ndn-cxx/face.hpp>
#include <boost/asio/io_service.hpp>
#include "DirectoryCrawler.h"
#include <thread>

using namespace ndn;
using namespace std;

class FileRepo {
public:
    FileRepo(Face &face, string homeName, string nodeName, DirectoryCrawler *directoryCrawler, string fileListLocation, string homeCertificateName);
    void stop();

private:
    void setInterestFilter();
    RegisteredPrefixHandle registeredPrefix;
    boost::asio::io_service m_ioService;
    Face face;
    string homeName;
    string nodeName;
    void onInterest(const Interest& interest);
    DirectoryCrawler *directoryCrawler;
    KeyChain keyChain;
    thread m_thread;
    string fileListLocation;
    string homeCertificateName;
};


#endif //NDN_DROP_FILEREPO_H

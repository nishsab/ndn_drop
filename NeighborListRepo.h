//
// Created by NIshant Sabharwal on 9/26/20.
//

#ifndef NDN_DROP_NEIGHBORLISTREPO_H
#define NDN_DROP_NEIGHBORLISTREPO_H

#include <ndn-cxx/face.hpp>
#include <boost/asio/io_service.hpp>
#include "NeighborList.h"

using namespace ndn;
using namespace std;

class NeighborListRepo {
public:
    NeighborListRepo(string homeName, string nodeName, NeighborList *neighborList);
    void stop();

private:
    void setInterestFilter();
    RegisteredPrefixHandle registeredPrefix;
    boost::asio::io_service m_ioService;
    Face face;
    string homeName;
    string nodeName;
    void onInterest(const Interest& interest);
    NeighborList *neighborList;
    KeyChain keyChain;
    thread m_thread;
};


#endif //NDN_DROP_NEIGHBORLISTREPO_H

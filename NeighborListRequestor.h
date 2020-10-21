//
// Created by NIshant Sabharwal on 10/11/20.
//

#ifndef NDN_DROP_NEIGHBORLISTREQUESTOR_H
#define NDN_DROP_NEIGHBORLISTREQUESTOR_H

#include <ndn-cxx/face.hpp>
#include "NeighborList.h"
#include <boost/asio/io_service.hpp>

class NeighborListRequestor {
public:
    NeighborListRequestor(int heartbeatWindow, string homeName, string nodeName, NeighborList *neighborList);
    void stop();

private:
    void handleNeighborListResponse(const Interest&, const Data& data);
    void onNack();
    void onTimeout();
    void requestNeighborList();
    void run();
    void threadRunner();
    string homeName;
    string nodeName;
    NeighborList *neighborList;
    bool running;
    boost::asio::io_service m_ioService;
    thread requestorThread;
    Face m_face;
    Scheduler m_scheduler;
    KeyChain m_keyChain;
    int heartbeatWindow;
};


#endif //NDN_DROP_NEIGHBORLISTREQUESTOR_H

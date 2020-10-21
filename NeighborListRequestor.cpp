//
// Created by NIshant Sabharwal on 10/11/20.
//

#include <iostream>
#include "NeighborListRequestor.h"
#include "Utils.h"
#include "NeighborListName.h"
#include <ndn-cxx/security/signing-helpers.hpp>
#include <boost/date_time.hpp>

namespace bpt = boost::posix_time;

using namespace std;
using namespace ndn;

void NeighborListRequestor::handleNeighborListResponse(const Interest&, const Data& data)
{
    const Block& content = data.getContent();
    string val = string(content.value(), content.value() + content.value_size());

    Utils::logf("NeighborListRequestor::handleNeighborListResponse: %s\n", val.c_str());
    neighborList->deserializeList(val);
    run();
}

void NeighborListRequestor::onNack()
{
    Utils::errf("NeighborListRequestor::onNack: Received a nack.\n");
    run();
}

void NeighborListRequestor::onTimeout()
{
    Utils::errf("NeighborListRequestor::onTimeout: Received a timeout.\n");
    run();
}

void NeighborListRequestor::requestNeighborList() {
    NeighborListName neighborListName = NeighborListName(this->homeName, this->nodeName);
    Name name = neighborListName.getNeighborListName(true);
    Utils::logf("NeighborListRequestor::requestNeighborList: Sending request for %s\n", name.toUri().c_str());
    Interest interest(name);
    interest.setCanBePrefix(true);
    interest.setMustBeFresh(true);
    m_face.expressInterest(interest,
                         bind(&NeighborListRequestor::handleNeighborListResponse, this,  _1, _2),
                         bind([this] { onNack(); }),
                         bind([this] { onTimeout(); }));
}

void NeighborListRequestor::run() {
    if (running) {
        Interest::setDefaultCanBePrefix(true);
        m_scheduler.schedule(time::seconds{this->heartbeatWindow}, bind(&NeighborListRequestor::requestNeighborList, this));
    }
}

void NeighborListRequestor::threadRunner() {
    run();
    m_face.getIoService().run();
    while (running) {
        sleep(1);
    }
}

void NeighborListRequestor::stop() {
    running = false;
    requestorThread.join();
}

NeighborListRequestor::NeighborListRequestor(int heartbeatWindow, string homeName, string nodeName, NeighborList *neighborList) : m_face(), m_scheduler(m_face.getIoService()) /*m_face(m_ioService), m_scheduler(m_ioService)*/
{
    this->homeName = homeName;
    this->nodeName = nodeName;
    this->heartbeatWindow = heartbeatWindow;
    this->neighborList = neighborList;
    running = true;
    requestorThread = thread(&NeighborListRequestor::threadRunner, this);
}

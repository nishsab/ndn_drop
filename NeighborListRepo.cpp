//
// Created by NIshant Sabharwal on 9/26/20.
//

#include <iostream>
#include "NeighborListRepo.h"
#include "NeighborListName.h"
#include "Utils.h"
#include <vector>
#include "Config.h"

using namespace std;

void NeighborListRepo::onInterest(const Interest& interest)
{
    Utils::logf("Received an interest: %s\n", interest.getName().toUri().c_str());

    try {
        NeighborListName neighborListName = NeighborListName(interest.getName().toUri());
        if (neighborListName.sender.compare(nodeName) == 0) {
            Utils::logf("Ignoring message from self!\n");
            return;
        }

        neighborList->addNeighbor(neighborListName.sender);

        // random backoff
        int microseconds = rand() % 750000;
        usleep(microseconds);

        string neighbors = neighborList->serializeList();
        std::vector<uint8_t> buffer(neighbors.begin(), neighbors.end());
        auto data = make_shared<Data>(interest.getName());
        data->setFreshnessPeriod(1_s);
        data->setContent(buffer.data(), static_cast<size_t>(neighbors.length()));

        keyChain.sign(*data);
        face.put(*data);
    }
    catch (int e) {
        Utils::errf("Error: invalid discover name: %s\n", interest.getName().toUri().c_str());
    }
}

void NeighborListRepo::setInterestFilter() {
    NeighborListName neighborListName = NeighborListName(this->homeName, this->nodeName);
    Name discoverPrefix = neighborListName.getNeighborListName(false);
    Utils::logf("Listening for neighbor list requests on %s\n", discoverPrefix.toUri().c_str());
    registeredPrefix = face.setInterestFilter(
            discoverPrefix,
            bind(&NeighborListRepo::onInterest, this, _2),
            [] (const auto&, const auto& reason) {
                NDN_THROW(std::runtime_error("Failed to register prefix: " + reason));
            });
    face.processEvents();
    cout << "after filter" << endl;
}

void NeighborListRepo::stop() {
    m_thread.join();
}

NeighborListRepo::NeighborListRepo(Face &face, string homeName, string nodeName, NeighborList *neighborList) : m_face(face), face(m_ioService) {
    //face.getIoService().run();
    m_ioService.run();
    cout << "a" << endl;
    this->homeName = homeName;
    cout << "b" << endl;
    this->nodeName = nodeName;
    cout << "c" << endl;
    this->neighborList = neighborList;
    cout << "d" << endl;

    m_thread = thread(&NeighborListRepo::setInterestFilter, this);
    cout << "e" << endl;
}


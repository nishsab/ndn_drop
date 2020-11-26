//
// Created by NIshant Sabharwal on 9/14/20.
//

#include "NeighborList.h"
#include "Utils.h"
#include <sstream>
#include <vector>

using namespace std;

void NeighborList::addNeighbor(std::string neighborName, int timeRemaining) {
    neighborList[neighborName] = timeRemaining;
}

void NeighborList::addNeighbor(std::string neighborName) {
    neighborList[neighborName] = neighborListLifetime;
}

void NeighborList::decrementList() {
    neighborListLock.lock();
    for (auto it = neighborList.begin(); it != neighborList.end(); ) {
        if (it->second > 0) {
            it->second--;
            it++;
        }
        else {
            it = neighborList.erase(it);
        }
    }
    neighborListLock.unlock();
}

string NeighborList::serializeList() {
    neighborListLock.lock();
    stringstream ss;
    for (auto x : neighborList) {
        ss << x.first << ":" << x.second << "\n";
    }
    string serialized = ss.str();
    if (serialized.length() > 0) {
        serialized.pop_back();
    }
    neighborListLock.unlock();
    return serialized;
};

string NeighborList::getNeighborsJson() {
    stringstream ss;
    bool hasValues = false;
    ss << "{\"status\": \"success\", \"neighbors\": [";
    neighborListLock.lock();
    for (auto x : neighborList) {
        hasValues = true;
        ss << "\"" << x.first << "\",";
    }
    string serialized = ss.str();
    if (hasValues) {
        serialized.pop_back();
    }
    neighborListLock.unlock();
    return serialized + "]}";
};

void NeighborList::deserializeList(std::string blob) {
    if (blob.length() == 0) {
        return;
    }
    vector<std::string> blobs = Utils::split(blob, '\n');
    neighborListLock.lock();
    for (int i=0; i<blobs.size(); i++) {
        vector<string> nameCount = Utils::split(blobs[i], ':');
        if (nameCount.size() == 2) {
            addNeighbor(nameCount[0], stoi(nameCount[1]));
        }
    }
    neighborListLock.unlock();
};

void NeighborList::monitorNeighborList() {
    while (running) {
        decrementList();
        sleep(this->heartbeatWindow);
    }
}

void NeighborList::stop() {
    running = false;
    neighborListThread.join();
}

NeighborList::NeighborList (int heartbeatWindow) {
    running = true;
    this->heartbeatWindow = heartbeatWindow;
    neighborListThread = thread(&NeighborList::monitorNeighborList, this);
}
//
// Created by NIshant Sabharwal on 10/14/20.
//

#ifndef NDN_DROP_NEIGHBORLIST_H
#define NDN_DROP_NEIGHBORLIST_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <ndn-cxx/util/scheduler.hpp>

using namespace std;
using namespace ndn;

class NeighborList {
public:
    NeighborList(int heartbeatWindow);
    void stop();
    vector<string> listNeighbors();
    void addNeighbor(string neighborName);
    string serializeList();
    void deserializeList(string blob);
    string getNeighborsJson();

private:
    void addNeighbor(string neighborName, int timeRemaining);
    void decrementList();
    int heartbeatWindow = 3;
    int neighborListLifetime = 6;
    unordered_map<string, int> neighborList;
    void monitorNeighborList();
    mutex neighborListLock;
    thread neighborListThread;
    bool running;
};

#endif //NDN_DROP_NEIGHBORLIST_H

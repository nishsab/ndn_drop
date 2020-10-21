//
// Created by NIshant Sabharwal on 10/11/20.
//

#include <iostream>
#include "NeighborListName.h"
#include "Utils.h"

NeighborListName::NeighborListName(string name) {
    vector<string> tokens = Utils::split(name.substr(1), '/');
    if (tokens.size() < MAX) {
        throw -1;
    }
    home = tokens.at(HOME);
    sender = tokens.at(SENDER);
}

NeighborListName::NeighborListName(string home, string sender) {
    this->home = home;
    this->sender = sender;
}

Name NeighborListName::getNeighborListName(bool request) {
    // "ndn/broadcast/drop/discover/nishant/laptop/neighbor_list.txt"
    Name name("ndn/broadcast/drop/discover");
    name.append(this->home);
    if (request) {
        name.append(this->sender);
        name.append("neighbor_lists.txt");
    }
    return name;
}
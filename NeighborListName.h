//
// Created by NIshant Sabharwal on 10/11/20.
//

#ifndef NDN_DROP_NEIGHBORLISTNAME_H
#define NDN_DROP_NEIGHBORLISTNAME_H

#include <string>
#include <vector>
#include <ndn-cxx/name.hpp>

using namespace std;
using namespace ndn;

enum NeighborListNameParts
{
    NDN = 0,
    BROADCAST,
    DROP,
    DISCOVER,
    HOME,
    SENDER,
    FILENAME,
    MAX
};

// "ndn/broadcast/drop/discover/nishant/laptop/neighbor_list.txt"
class NeighborListName {
public:

    NeighborListName(string name);
    NeighborListName(string home, string sender);
    Name getNeighborListName(bool request);
    string home;
    string sender;
};


#endif //NDN_DROP_NEIGHBORLISTNAME_H

//
// Created by NIshant Sabharwal on 11/13/20.
//

#ifndef NDN_DROP_PACKETSENDER_H
#define NDN_DROP_PACKETSENDER_H
#include <ndn-cxx/data.hpp>
#include <vector>

using namespace ndn;
using namespace std;

class PacketSender {
public:
    PacketSender(string repoHostname, int port);
    void sendPackets(vector<const Data> dataPackets);

private:
    string repoHostname;
    int port;
};


#endif //NDN_DROP_PACKETSENDER_H

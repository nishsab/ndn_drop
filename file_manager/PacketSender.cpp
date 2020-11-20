//
// Created by NIshant Sabharwal on 11/13/20.
//

#include "PacketSender.h"
#include "../PracticalSocket.h"  // For Socket and SocketException
#include <iostream>           // For cerr and cout
#include <cstdlib>            // For atoi()
#include "../Utils.h"

PacketSender::PacketSender(string repoHostname, int port) {
    this->repoHostname = repoHostname;
    this->port = port;
}

void PacketSender::sendPackets(vector<const Data> dataPackets) {
    try {
        TCPSocket sock(repoHostname, port);
        for (Data dataPacket : dataPackets) {
            Utils::logf("PacketSender::sendPackets: Sending packet: %s\n", dataPacket.getName().toUri().c_str());
            Block block = dataPacket.wireEncode();
            sock.send((char *) block.wire(), block.size());
        }
    } catch(SocketException &e) {
        cerr << e.what() << endl;
        cout << repoHostname << " " << port << endl;
        exit(1);
    }
}
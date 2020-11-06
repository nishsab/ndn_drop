//
// Created by NIshant Sabharwal on 11/4/20.
//

#ifndef NDN_DROP_FILEDOWNLOADER_H
#define NDN_DROP_FILEDOWNLOADER_H

#include <string>
#include <ndn-cxx/face.hpp>
#include "NeighborList.h"
#include <boost/asio/io_service.hpp>

using namespace std;
using namespace ndn;

struct CallbackContainer {
    ofstream *stream;
    mutex lock;
    int counter;
};

class FileDownloader {
public:
    FileDownloader(string inboundDirectoryPath);
    string getFile(string ndnName, int numBlocks, string filename, int fileSize, int blockSize);

private:
    void handleFileResponse(const Interest&, const Data& data, CallbackContainer *callbackContainer, int blockid, int blockSize);
    void onNack();
    void onTimeout();
    Face m_face;
    string inboundDirectoryPath;
};


#endif //NDN_DROP_FILEDOWNLOADER_H

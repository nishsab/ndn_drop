//
// Created by NIshant Sabharwal on 10/25/20.
//

#ifndef NDN_DROP_FILEREQUESTOR_H
#define NDN_DROP_FILEREQUESTOR_H

#include <ndn-cxx/face.hpp>
#include "NeighborList.h"
#include <boost/asio/io_service.hpp>

struct CallbackBlocker {
    string val;
};

class FileRequestor {
public:
    FileRequestor(string homeName, string nodeName);
    string getFileList(string owner);
    bool getFile(string ndnFileName, string outputPath);

private:
    void handleFileListResponse(const Interest&, const Data& data, CallbackBlocker *callbackBlocker);
    void handleGetFileResponse(const Interest&, const Data& data, string outputPath, CallbackBlocker *callbackBlocker);
    void onNack(CallbackBlocker *blocker);
    void onTimeout(CallbackBlocker *callbackBlocker);
    string homeName;
    Face m_face;
};


#endif //NDN_DROP_FILEREQUESTOR_H

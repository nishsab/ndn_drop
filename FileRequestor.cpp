//
// Created by NIshant Sabharwal on 10/25/20.
//

#include <iostream>
#include "FileRequestor.h"
#include "Utils.h"
#include "FileName.h"

using namespace std;

FileRequestor::FileRequestor(string homeName, string nodeName)
{
    this->homeName = homeName;
}

string FileRequestor::getFileList(string owner) {
    CallbackBlocker blocker;
    FileName fileName = FileName(homeName, owner);
    Name fileListname = fileName.getFileListName();
    Utils::logf("FileRequestor::getFileList: Sending request for %s\n", fileListname.toUri().c_str());
    Interest interest(fileListname);
    interest.setCanBePrefix(true);
    interest.setMustBeFresh(true);
    m_face.expressInterest(interest,
                           bind(&FileRequestor::handleFileListResponse, this,  _1, _2, &blocker),
                           bind(&FileRequestor::onNack, this, &blocker),
                           bind(&FileRequestor::onTimeout, this, &blocker));

    m_face.processEvents();
    return blocker.val;
}

void FileRequestor::handleFileListResponse(const Interest&, const Data& data, CallbackBlocker *callbackBlocker)
{
    const Block& content = data.getContent();
    string val = string(content.value(), content.value() + content.value_size());

    Utils::logf("FileRequestor::handleFileListResponse: %s\n", val.c_str());
    callbackBlocker->val = val;
}

void FileRequestor::onNack(CallbackBlocker *callbackBlocker)
{
    Utils::errf("FileRequestor::onNack: Received a nack.\n");
    callbackBlocker->val = "{\"status\": \"error\", \"reason\": \"Received a nack.\"}";
}

void FileRequestor::onTimeout(CallbackBlocker *callbackBlocker)
{
    Utils::errf("FileRequestor::onNack: Received a nack.\n");
    callbackBlocker->val = "{\"status\": \"error\", \"reason\": \"Received a nack.\"}";
}
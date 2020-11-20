//
// Created by NIshant Sabharwal on 10/25/20.
//

#include <iostream>
#include <fstream>
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

bool FileRequestor::getFile(string ndnFileName, string outputPath) {
    Name ndnName = Name(ndnFileName);
    Utils::logf("FileRequestor::getFile: Sending request for %s\n", ndnName.toUri().c_str());
    Interest interest(ndnName);
    interest.setCanBePrefix(true);
    interest.setMustBeFresh(false);

    CallbackBlocker blocker;
    m_face.expressInterest(interest,
                           bind(&FileRequestor::handleGetFileResponse, this,  _1, _2, outputPath, &blocker),
                           bind(&FileRequestor::onNack, this, &blocker),
                           bind(&FileRequestor::onTimeout, this, &blocker));
    m_face.processEvents();

    return blocker.val.find("success") != string::npos;
}

void FileRequestor::handleFileListResponse(const Interest&, const Data& data, CallbackBlocker *callbackBlocker)
{
    const Block& content = data.getContent();
    string val = string(content.value(), content.value() + content.value_size());

    Utils::logf("FileRequestor::handleFileListResponse: %s\n", val.c_str());
    callbackBlocker->val = val;
}

void FileRequestor::handleGetFileResponse(const Interest&, const Data& data, string outputPath, CallbackBlocker *callbackBlocker)
{
    cout << "got file!" << endl;
    Utils::logf("FileRequestor::handleGetFileResponse: %s\n", data.getName().toUri().c_str());
    const Block& content = data.getContent();
    cout << content.size() << endl;
    ConstBufferPtr buf = content.getBuffer();
    cout << buf->size() << endl;
    ofstream fout;
    fout.open(outputPath, ios::binary | ios::out);
    fout.write((char*) content.value(), content.size());
    //fout.write((char*) (char *)buf->get<uint8_t>(), buf->size());
    fout.close();

    callbackBlocker->val = "{\"status\": \"success\"}";
}

void FileRequestor::onNack(CallbackBlocker *callbackBlocker)
{
    cout << "got nack!" << endl;
    Utils::errf("FileRequestor::onNack: Received a nack.\n");
    callbackBlocker->val = "{\"status\": \"error\", \"reason\": \"Received a nack.\"}";
}

void FileRequestor::onTimeout(CallbackBlocker *callbackBlocker)
{
    cout << "got timeout!" << endl;
    Utils::errf("FileRequestor::onNack: Received a nack.\n");
    callbackBlocker->val = "{\"status\": \"error\", \"reason\": \"Received a nack.\"}";
}
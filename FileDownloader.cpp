//
// Created by NIshant Sabharwal on 11/4/20.
//

#include <fstream>
#include "FileDownloader.h"
#include "Utils.h"

FileDownloader::FileDownloader(string inboundDirectoryPath)
{
    this->inboundDirectoryPath = inboundDirectoryPath;
}

string FileDownloader::getFile(string ndnName, int numBlocks, string filename, int fileSize, int blockSize) {
    string fullPath = string(inboundDirectoryPath);
    if (fullPath.size() > 0) {
        fullPath.append("/");
    }
    fullPath.append(filename);
    Utils::logf("FileDownloader::getFile: Downloading %s to %s\n", ndnName.c_str(), fullPath.c_str());

    ofstream ofs(fullPath, std::ios::binary | std::ios::out);
    ofs.seekp(fileSize-1);
    ofs.write("", 1);
    CallbackContainer callbackContainer = {0};
    callbackContainer.stream = &ofs;

    for (int i=0; i<numBlocks; i++) {
        Name segmentName = Name(ndnName);
        ostringstream segmentStream;
        segmentStream << "seg=" << i;
        segmentName.append(segmentStream.str());
        Utils::logf("FileDownloader::getFile: Sending request for %s\n", segmentName.toUri().c_str());
        Interest interest(segmentName);
        interest.setCanBePrefix(true);
        interest.setMustBeFresh(false);
        m_face.expressInterest(interest,
                               bind(&FileDownloader::handleFileResponse, this,  _1, _2, &callbackContainer, i, blockSize),
                               bind(&FileDownloader::onNack, this),
                               bind(&FileDownloader::onTimeout, this));
    }
    m_face.processEvents();
    ofs.close();

    if (callbackContainer.counter == numBlocks) {
        return "{\"status\": \"success\"}";
    }
    else {
        ostringstream os;
        os << "{\"status\": \"error\", \"reason\": \"Received " << callbackContainer.counter << " of " << numBlocks << " segments.\"}";
        return os.str();
    }
}

void FileDownloader::handleFileResponse(const Interest&, const Data& data, CallbackContainer *callbackContainer, int blockId, int blockSize)
{
    Utils::logf("FileDownloader::handleFileResponse: Received data packet %s.\n", data.getName().toUri().c_str());

    callbackContainer->lock.lock();
    const Block& content = data.getContent();
    callbackContainer->stream->seekp(blockId*blockSize);
    callbackContainer->stream->write((char *) content.value(), content.value_size());

    callbackContainer->counter++;
    callbackContainer->lock.unlock();
}

void FileDownloader::onNack()
{
    Utils::errf("FileDownloader::onNack: Received a nack.\n");
}

void FileDownloader::onTimeout()
{
    Utils::errf("FileDownloader::onTimeout: Received a timeout.\n");
}
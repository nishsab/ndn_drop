//
// Created by NIshant Sabharwal on 11/4/20.
//

#include <fstream>
#include <iostream>
#include "FileDownloader.h"
#include "Utils.h"

FileDownloader::FileDownloader(string inboundDirectoryPath,
                               string homeCertificateName,
                               string schemaConfPath,
                               string homeName,
                               string pibLocator,
                               string tpmLocator)
: keyChain(pibLocator, tpmLocator),
  m_face(m_ioService),
  m_validator(m_face),
  m_decryptor(keyChain.getPib().getIdentity(Name(homeCertificateName).getPrefix(-4)).getKey(Name(homeCertificateName).getPrefix(-2)),
                      m_validator, keyChain, m_face)
{
    m_validator.load(schemaConfPath);
    this->inboundDirectoryPath = inboundDirectoryPath;
}

string FileDownloader::getFile(string ndnName, int numBlocks, string filename, int fileSize, int blockSize, string owner) {
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
        Name segmentName = Name(ndnName).appendSegment(i);
        Utils::logf("FileDownloader::getFile: Sending request for %s\n", segmentName.toUri().c_str());
        Interest interest(segmentName);
        interest.setCanBePrefix(true);
        interest.setMustBeFresh(false);
        m_face.expressInterest(interest,
                               bind(&FileDownloader::handleFileResponse, this,  _1, _2, &callbackContainer, i, blockSize),
                               bind(&FileDownloader::onNack, this),
                               bind(&FileDownloader::onTimeout, this));

        m_face.processEvents();
    }
    ofs.close();

    if (callbackContainer.counter == numBlocks) {
        return "{\"status\": \"success\"}";
    }
    else if (callbackContainer.error) {
        return "{\"status\": \"error\", \"reason\": \"" + callbackContainer.errorMessage + "\"}";
    }
    else {
        ostringstream os;
        os << "{\"status\": \"error\", \"reason\": \"Received " << callbackContainer.counter << " of " << numBlocks << " segments.\"}";
        return os.str();
    }
}

void FileDownloader::afterValidation(const Data& data, CallbackContainer *callbackContainer, int blockId, int blockSize) {
    Utils::logf("FileDownloader::afterValidation: validation succeeded.\n");
    m_decryptor.decrypt(data.getContent().blockFromValue(),
                        [=] (ConstBufferPtr content) {
                            string decryptedContent = std::string(reinterpret_cast<const char*>(content->data()), content->size());
                            Utils::logf("FileDownloader::afterValidation: successfully decrypted content.\n");
                            callbackContainer->lock.lock();
                            callbackContainer->stream->seekp(blockId*blockSize);
                            callbackContainer->stream->write((char *) decryptedContent.c_str(), decryptedContent.size());
                            callbackContainer->counter++;
                            callbackContainer->lock.unlock();
                        },
                        [=] (const ErrorCode&, const std::string& error) {
                            Utils::errf("FileDownloader::afterValidation: Received a decrypyion error: %s.\n", error.c_str());
                            callbackContainer->error = true;
                            callbackContainer->errorMessage = "Cannot decrypt retrieved data: " + string(error);
                        });
    Utils::logf("FileDownloader::afterValidation: done decrypting.\n");
}

void FileDownloader::validationError(const ndn::security::ValidationError &error, CallbackContainer *callbackContainer) {
    Utils::errf("Error validating data: %s\n", error.getInfo().c_str());
    callbackContainer->error = true;
    callbackContainer->errorMessage = "Cannot validate retrieved data: " + string(error.getInfo());
}

void FileDownloader::handleFileResponse(const Interest&, const Data& data, CallbackContainer *callbackContainer, int blockId, int blockSize)
{
    Utils::logf("FileDownloader::handleFileResponse: Received data packet %s.\n", data.getName().toUri().c_str());

    m_validator.validate(data,
                         bind(&FileDownloader::afterValidation, this, _1, callbackContainer, blockId, blockSize),
                         bind(&FileDownloader::validationError, this, _2, callbackContainer));
}

void FileDownloader::onNack()
{
    Utils::errf("FileDownloader::onNack: Received a nack.\n");
}

void FileDownloader::onTimeout()
{
    Utils::errf("FileDownloader::onTimeout: Received a timeout.\n");
}
//
// Created by NIshant Sabharwal on 11/4/20.
//

#include <fstream>
#include <iostream>
#include "FileDownloader.h"
#include "Utils.h"

FileDownloader::FileDownloader(string inboundDirectoryPath, SecurityPackage *securityPackage, string homeCertificateName, string schemaConfPath, string homeName)
        : m_validator(m_face),
          m_decryptor(keyChain.getPib().getIdentity(Name(homeCertificateName).getPrefix(-4)).getKey(Name(homeCertificateName).getPrefix(-2)), m_validator, keyChain, m_face),
          keyRequestor(schemaConfPath, homeCertificateName)
{
    m_validator.load(schemaConfPath);
    this->inboundDirectoryPath = inboundDirectoryPath;
    this->securityPackage = securityPackage;
    this->homeName = homeName;
}

string FileDownloader::getFile(string ndnName, int numBlocks, string filename, int fileSize, int blockSize, string owner) {
    string message;
    keyRequestor.requestAccess(homeName, owner);
    return "";
    for (int i=0; i<2; i++) {
        cout << "Requesting access for " << owner << endl;
        bool succeeded = keyRequestor.requestAccess(homeName, owner);
        if (succeeded) {
            cout << "Access request succeeded" << endl;
        }
        else {
            cout << "Access request failed" << endl;
        }
        message = getFileWithAccess(ndnName, numBlocks, filename, fileSize, blockSize);
        cout << message << endl;
        if (message.find("success") != string::npos) {
            return message;
        }
    }
    return message;
}

string FileDownloader::getFileWithAccess(string ndnName, int numBlocks, string filename, int fileSize, int blockSize) {
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
        //Name segmentName = Name(ndnName);
        //ostringstream segmentStream;
        //segmentStream << "seg=" << i;
        //segmentName.append(segmentStream.str());
        Name segmentName = Name(ndnName).appendSegment(i);
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
    else if (callbackContainer.accessError) {
        return "{\"status\": \"error\", \"reason\": \"Could not decrypt!\"}";
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

    cout << "PRE" << endl;
    EncryptedContent encryptedContent(data.getContent().blockFromValue());
    cout << encryptedContent.getKeyLocator().toUri() << endl;
    m_decryptor.decrypt(data.getContent().blockFromValue(),
                                                 [=] (ConstBufferPtr content) {
                                                    cout << "SUCCESS!!!" << endl;
                                                     std::cout << "Decrypted content: "
                                                               << std::string(reinterpret_cast<const char*>(content->data()), content->size())
                                                               << std::endl;

                                                     cout << "POST" << endl;
                                                     callbackContainer->lock.lock();
                                                     callbackContainer->stream->seekp(blockId*blockSize);
                                                     callbackContainer->stream->write((char *) content.get(), content->size());

                                                     callbackContainer->counter++;
                                                     callbackContainer->lock.unlock();
                                                 },
                                                 [=] (const ErrorCode&, const std::string& error) {
                                                     cout << "FAILURE!!!" << endl;
                                                     std::cerr << "Cannot decrypt data: " << error << std::endl;
                                                     callbackContainer->accessError = true;
                                                 });
    sleep(2);

    /*cout << "POST" << endl;
    callbackContainer->lock.lock();
    const Block& content = data.getContent();
    callbackContainer->stream->seekp(blockId*blockSize);
    callbackContainer->stream->write((char *) content.value(), content.value_size());

    callbackContainer->counter++;
    callbackContainer->lock.unlock();*/
}

void FileDownloader::onNack()
{
    Utils::errf("FileDownloader::onNack: Received a nack.\n");
}

void FileDownloader::onTimeout()
{
    Utils::errf("FileDownloader::onTimeout: Received a timeout.\n");
}
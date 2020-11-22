#ifdef _MSC_VER
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <stdlib.h>
#include <signal.h>
#include <mongoose/Server.h>
#include <mongoose/WebController.h>
#include "NeighborList.h"
#include "NeighborListRepo.h"
#include "NeighborListRequestor.h"
#include "NeighborListName.h"
#include "Conf.h"
#include "DirectoryCrawler.h"
#include "FileRepo.h"
#include "FileRequestor.h"
#include "FileDownloader.h"
#include "KeyRepo.h"
#include "KeyRequestor.h"
#include "file_manager/DirectoryManager.h"
#include "SecurityPackage.h"

using namespace std;
using namespace Mongoose;

class MyController : public WebController
{
public:
    void hello(Request &request, StreamResponse &response)
    {
        response << "Hello " << htmlEntities(request.get("name", "... what's your name ?")) << endl;
    }

    void getNeighbors(Request &request, StreamResponse &response)
    {
        string neighbors = neighborList->getNeighborsJson();
        response << neighbors;
    }

    void getFileList(Request &request, StreamResponse &response)
    {
        string owner = request.get("owner", "");
        string fileList = fileRequestor->getFileList(owner);
        response << fileList;
    }

    void getFile(Request &request, StreamResponse &response)
    {
        string ndnName = request.get("ndn_name", "");
        string numBlocks = request.get("num_blocks", "");
        string filename = request.get("file_name", "");
        string fileSize = request.get("file_size", "");
        string blockSize = request.get("block_size", "");
        string owner = request.get("owner", "");
        if (ndnName.empty() || filename.empty() || numBlocks.empty() || fileSize.empty() || blockSize.empty() || owner.empty()) {
            response << "{\"status\": \"error\", \"reason\": \"ndn_name, file_name, num_blocks, owner and block_size are required arguments.\"}";
        }
        else {
            string status = fileDownloader->getFile(ndnName, stoi(numBlocks), filename, stoi(fileSize), stoi(blockSize), owner);
            response << status;
        }
    }

    void getSessionKey(Request &request, StreamResponse &response)
    {
        string name = request.get("name", "");
        if (name.empty()) {
            response << "{\"status\": \"error\", \"reason\": \"name are required arguments.\"}";
        }
        else {
            string fileList = keyRequestor->getFileList(name);
            response << fileList;
        }
    }

    void setup()
    {
        addRoute("GET", "/hello", MyController, hello);
        addRoute("GET", "/get_neighbors", MyController, getNeighbors);
        addRoute("GET", "/get_file_list", MyController, getFileList);
        addRoute("GET", "/get_file", MyController, getFile);
        addRoute("GET", "/get_session_key", MyController, getSessionKey);
    }

    MyController(string home, string node, Conf conf) {
        /*securityPackage = new SecurityPackage(conf.pibLocator,
                                              conf.tpmLocator,
                                              conf.homeCertificateName,
                                              conf.schemaConfPath,
                                              conf.nacIdentityName,
                                              conf.nacDataName,
                                              conf.nacAccessPrefix,
                                              conf.nacCkPrefix);*/
        neighborList = new NeighborList(conf.heartbeatWindow);
        neighborListRepo = new NeighborListRepo(home, node, neighborList);
        neighborListRequestor = new NeighborListRequestor(conf.heartbeatWindow, home, node, neighborList);
        directoryManager = new DirectoryManager(conf.outboundDirectory,
                                                conf.blockSize,
                                                conf.filePrefix,
                                                conf.repoHostName,
                                                conf.repoPort,
                                                conf.homeCertificateName,
                                                conf.pibLocator,
                                                conf.tpmLocator,
                                                conf.nacAccessPrefix,
                                                conf.nacCkFilePrefix,
                                                conf.schemaConfPath);
        fileRepo = new FileRepo(conf.pibLocator,
                                conf.tpmLocator,
                                home,
                                node,
                                directoryManager,
                                conf.homeCertificateName,
                                conf.schemaConfPath,
                                conf.nacIdentityName,
                                conf.nacDataName,
                                conf.nacAccessPrefix,
                                conf.nacCkPrefix);

        cout << "done with directory manager" << endl;
        fileRequestor = new FileRequestor(home,
                                          conf.schemaConfPath,
                                          conf.homeCertificateName,
                                          conf.pibLocator,
                                          conf.tpmLocator);
        fileDownloader = new FileDownloader(conf.inboundDirectory,
                                            conf.homeCertificateName,
                                            conf.schemaConfPath,
                                            home,
                                            conf.pibLocator,
                                            conf.tpmLocator);
    }

private:
    NeighborList *neighborList;
    NeighborListRepo *neighborListRepo;
    NeighborListRequestor *neighborListRequestor;
    FileRepo *fileRepo;
    FileRequestor *fileRequestor;
    FileDownloader *fileDownloader;
    KeyRepo *keyRepo;
    KeyRequestor *keyRequestor;
    DirectoryManager *directoryManager;
    SecurityPackage *securityPackage;
};


int main(int argc, char* argv[])
{
    if (argc != 5) {
        cout << "Usage: ./main port home node conf" << endl;
        return 0;
    }
    int port = atoi(argv[1]);
    string home = argv[2];
    string node = argv[3];
    string confPath = argv[4];
    Conf conf = Conf(confPath);
    cout << "Starting endpoints on port " << port << ". Home: " << home << " Node: " << node << " Conf: " << confPath << endl;
    MyController myController = MyController(home, node, conf);
    Server server(port);
    server.registerController(&myController);

    server.start();
    while (1) {
        sleep(10);
    }
}

#ifdef _MSC_VER
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <stdlib.h>
#include <mongoose/Server.h>
#include <mongoose/WebController.h>
#include "NeighborList.h"
#include "NeighborListRepo.h"
#include "NeighborListRequestor.h"
#include "Conf.h"
#include "FileRepo.h"
#include "FileRequestor.h"
#include "FileDownloader.h"
#include "file_manager/DirectoryManager.h"

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
        string neighbors = neighborList.getNeighborsJson();
        response << neighbors;
    }

    void getFileList(Request &request, StreamResponse &response)
    {
        string owner = request.get("owner", "");
        if (owner.empty()) {
            response << "{\"status\": \"error\", \"reason\": \"owner are required arguments.\"}";
        }
        else {
            string fileList = fileRequestor.getFileList(owner);
            response << fileList;
        }
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
            string status = fileDownloader.getFile(ndnName, stoi(numBlocks), filename, stoi(fileSize), stoi(blockSize), owner);
            response << status;
        }
    }

    void setup()
    {
        addRoute("GET", "/hello", MyController, hello);
        addRoute("GET", "/get_neighbors", MyController, getNeighbors);
        addRoute("GET", "/get_file_list", MyController, getFileList);
        addRoute("GET", "/get_file", MyController, getFile);
    }

    MyController(string home, string node, Conf conf)
    : neighborList(conf.heartbeatWindow),
      neighborListRepo(home, node, &neighborList),
      neighborListRequestor(conf.heartbeatWindow, home, node, &neighborList),
      directoryManager(conf.outboundDirectory,
                       conf.blockSize,
                       conf.filePrefix,
                       conf.repoHostName,
                       conf.repoPort,
                       conf.homeCertificateName,
                       conf.pibLocator,
                       conf.tpmLocator,
                       conf.nacAccessPrefix,
                       conf.nacCkFilePrefix,
                       conf.schemaConfPath),
      fileRepo(conf.pibLocator,
               conf.tpmLocator,
               home,
               node,
               &directoryManager,
               conf.homeCertificateName,
               conf.schemaConfPath,
               conf.nacIdentityName,
               conf.nacDataName,
               conf.nacAccessPrefix,
               conf.nacCkPrefix),
      fileRequestor(home,
                    conf.schemaConfPath,
                    conf.homeCertificateName,
                    conf.pibLocator,
                    conf.tpmLocator),
      fileDownloader(conf.inboundDirectory,
                     conf.homeCertificateName,
                     conf.schemaConfPath,
                     conf.pibLocator,
                     conf.tpmLocator)
    {

    }

private:
    NeighborList neighborList;
    NeighborListRepo neighborListRepo;
    NeighborListRequestor neighborListRequestor;
    DirectoryManager directoryManager;
    FileRepo fileRepo;
    FileRequestor fileRequestor;
    FileDownloader fileDownloader;
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
    MyController *myController =  new MyController(home, node, conf);
    Server server(port);
    server.registerController(myController);

    server.start();
    while (1) {
        sleep(10);
    }
}

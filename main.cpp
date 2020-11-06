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
        if (ndnName.empty() || filename.empty() || numBlocks.empty() || fileSize.empty() || blockSize.empty()) {
            response << "{\"status\": \"error\", \"reason\": \"ndn_name, file_name, num_blocks and block_size are required arguments.\"}";
        }
        else {
            string status = fileDownloader->getFile(ndnName, stoi(numBlocks), filename, stoi(fileSize), stoi(blockSize));
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

    MyController(string home, string node, Conf conf) {
        neighborList = new NeighborList(conf.heartbeatWindow);
        Face face;
        neighborListRepo = new NeighborListRepo(face, home, node, neighborList);
        neighborListRequestor = new NeighborListRequestor(conf.heartbeatWindow, home, node, neighborList);
        DirectoryCrawler *directoryCrawler = new DirectoryCrawler(conf.outboundDirectory);
        fileRepo = new FileRepo(face, home, node, directoryCrawler, conf.fileListLocation);
        fileRequestor = new FileRequestor(home, node);
        fileDownloader = new FileDownloader(conf.inboundDirectory);
    }

private:
    NeighborList *neighborList;
    NeighborListRepo *neighborListRepo;
    NeighborListRequestor *neighborListRequestor;
    FileRepo *fileRepo;
    FileRequestor *fileRequestor;
    FileDownloader *fileDownloader;
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
    cout << "Starting endpoints on port " << port << ". Home: " << home << " Node: " << node << "Conf: " << confPath << endl;
    MyController myController = MyController(home, node, conf);
    Server server(port);
    server.registerController(&myController);

    server.start();
    while (1) {
        sleep(10);
    }
}

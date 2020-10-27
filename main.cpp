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

    void setup()
    {
        addRoute("GET", "/hello", MyController, hello);
        addRoute("GET", "/get_neighbors", MyController, getNeighbors);
        addRoute("GET", "/get_file_list", MyController, getFileList);
    }

    MyController(string home, string node, Conf conf) {
        neighborList = new NeighborList(conf.heartbeatWindow);
        Face face;
        neighborListRepo = new NeighborListRepo(face, home, node, neighborList);
        neighborListRequestor = new NeighborListRequestor(conf.heartbeatWindow, home, node, neighborList);
        DirectoryCrawler *directoryCrawler = new DirectoryCrawler(conf.outboundDirectory);
        fileRepo = new FileRepo(face, home, node, directoryCrawler);
        fileRequestor = new FileRequestor(home, node);
    }

private:
    NeighborList *neighborList;
    NeighborListRepo *neighborListRepo;
    NeighborListRequestor *neighborListRequestor;
    FileRepo *fileRepo;
    FileRequestor *fileRequestor;
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

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
#include "Config.h"
#include "NeighborListRepo.h"
#include "NeighborListRequestor.h"
#include "NeighborListName.h"

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

    void setup()
    {
        addRoute("GET", "/hello", MyController, hello);
        addRoute("GET", "/get_neighbors", MyController, getNeighbors);
    }

    MyController(string home, string node) {
        neighborList = new NeighborList(Config::heartbeatWindow);
        Face face;
        neighborListRepo = new NeighborListRepo(face, home, node, neighborList);
        neighborListRequestor = new NeighborListRequestor(Config::heartbeatWindow, home, node, neighborList);
    }

private:
    NeighborList *neighborList;
    NeighborListRepo *neighborListRepo;
    NeighborListRequestor *neighborListRequestor;
};


int main(int argc, char* argv[])
{
    if (argc != 4) {
        cout << "Usage: ./main port home node" << endl;
        return 0;
    }
    int port = atoi(argv[1]);
    string home = argv[2];
    string node = argv[3];
    cout << "Starting endpoints on port " << port << ". Home: " << home << " Node: " << node << endl;
    MyController myController = MyController(home, node);
    Server server(port);
    server.registerController(&myController);

    server.start();
    while (1) {
        sleep(10);
    }
}

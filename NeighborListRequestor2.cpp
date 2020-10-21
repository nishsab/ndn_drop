#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <boost/asio/io_service.hpp>
#include <functional>
#include <iostream>
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/scheduler.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>
#include <thread>

using namespace ndn;
using namespace std;

class NeighborListRequestor {
public:
    NeighborListRequestor()
            : m_face(m_ioService),
              m_scheduler(m_ioService)
    {
    }

    NeighborListRequestor(string homeName, string nodeName) : m_face(m_ioService),
                                                              m_scheduler(m_ioService) {
        this->homeName = homeName;
        this->nodeName = nodeName;
        running = true;
        Interest::setDefaultCanBePrefix(true);
        requestorThread = thread(&NeighborListRequestor::run, this);
        cout << "running" << endl;
        m_ioService.run();
    }

private:
    void
    after10sec()
    {
        std::cout << "\n******\nStart a new loop." << std::endl;
        Interest interest("/alice-home/TEMP/CONTENT/current/bedroom"); // bedroom current temperature content
        interest.setMustBeFresh(true);
        interest.setCanBePrefix(true);
        m_face.expressInterest(interest,
                               bind(&NeighborListRequestor::afterGetTemperature, this, _2),
                               bind([this] { onNack(); }),
                               bind([this] { onTimeout(); }));
    }

    void
    afterGetTemperature(const Data &data)
    {
        int temperature = *reinterpret_cast<const int *>(data.getContent().value());
        std::cout << "Temperature: " << temperature << std::endl;
        m_face.expressInterest(
                Interest("/alice-home/AIRCON/CONTENT/state/bedroom").setMustBeFresh(true), // bedroom aircon state
                bind(&NeighborListRequestor::afterGetAirconState, this, _2, temperature),
                bind([this] { restart(); }),
                bind([this] { restart(); }));
    }

    /** CS217B NDN Security Tutorial
     * @todo Demo how to use specific identity/key/certificate to sign the Interest packet.
     */
    void
    afterGetAirconState(const Data &data, int temperature)
    {
        std::string aircon_state(reinterpret_cast<const char *>(data.getContent().value()));
        Name aircon_command("/alice-home/AIRCON/CMD/");
        std::string action = "none";
        if (temperature < 65 && aircon_state != "heat") {
            action = "heat";
        }
        else if (temperature > 68 && aircon_state == "heat") {
            action = "off";
        }
        std::cout << "Aircon State: " << aircon_state << std::endl;
        std::cout << "Command Aircon to take action: " << action << std::endl;
        if (action != "none") {
            auto interest = Interest(aircon_command.append(action).append("bedroom")).setMustBeFresh(true);

            // sign Interest
            m_keyChain.sign(interest, security::signingByIdentity(Name("/alice-home")));

            m_face.expressInterest(interest, DataCallback(), NackCallback(), TimeoutCallback());
        }
        restart();
    }

    void
    restart()
    {
        std::cout << "Start to wanit 5s..." << std::endl;
        m_scheduler.schedule(5_s, bind(&NeighborListRequestor::after10sec, this));
    }

private:
    boost::asio::io_service m_ioService;
    Face m_face;
    Scheduler m_scheduler;
    KeyChain m_keyChain;

    void handleNeighborListResponse(const Data& data)
    {
        cout << "response" << endl;
        const Block& content = data.getContent();
        string val = string(content.value(), content.value() + content.value_size());
        run();
    }

    void onNack()
    {
        cout << "Nack" << endl;
        run();
    }

    void onTimeout()
    {
        cout << "timeout" << endl;
        run();
    }

    void requestNeighborList()
    {
        //NeighborListName neighborListName = NeighborListName(this->homeName, this->nodeName);
        //Name name = neighborListName.getNeighborListName(true);
        //Name name = Name("/ndn/drop/nishant/discover/home/neighbor_list.txt");
        std::cout << "\n******\nStart a new loop." << std::endl;
        Interest interest("/alice-home/TEMP/CONTENT/current/bedroom"); // bedroom current temperature content
        //cout << "Interest :" << interest << endl;
        interest.setCanBePrefix(true);
        interest.setMustBeFresh(true);
        m_face.expressInterest(interest,
                               bind(&NeighborListRequestor::handleNeighborListResponse, this, _2),
                               bind([this] { onNack(); }),
                               bind([this] { onTimeout(); }));
        //Interest interest("/alice-home/TEMP/CONTENT/current/bedroom"); // bedroom current temperature content
        //interest.setMustBeFresh(true);
        //interest.setCanBePrefix(true);
        /*m_face.expressInterest(interest,
                               bind(&NeighborListRequestor::handleNeighborListResponse, this, _2),
                               bind([this] { onNack(); }),
                               bind([this] { onTimeout(); }));*/
    }

    void run()
    {
        Interest::setDefaultCanBePrefix(true);
        //while (running) {
        //requestNeighborList();
        m_scheduler.schedule(2_s, bind(&NeighborListRequestor::requestNeighborList, this));
        //sleep(3);
        //}
    }
    string homeName;
    string nodeName;
    bool running;
    thread requestorThread;

};

int
main(int argc, char *argv[])
{
    if (argc != 4) {
        cout << "Usage: ./main port home node" << endl;
        return 0;
    }
    int port = atoi(argv[1]);
    string home = argv[2];
    string node = argv[3];
    cout << "Starting endpoints on port " << port << ". Home: " << home << " Node: " << node << endl;
    cout << "ola" << endl;

    NeighborListRequestor *neighborListRequestor = new NeighborListRequestor(home, node);
    while (1) {
        sleep(10);
    }
    return 0;
}
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <boost/asio/io_service.hpp>
#include <functional>
#include <iostream>
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/scheduler.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>
using namespace ndn;

class Controller {
public:
    Controller()
            : m_face(m_ioService),
              m_scheduler(m_ioService)
    {
    }

    void
    run()
    {
        Interest::setDefaultCanBePrefix(true);
        restart();
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
                               bind(&Controller::afterGetTemperature, this, _2),
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
                bind(&Controller::afterGetAirconState, this, _2, temperature),
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

    void onNack()
    {
        std::cout << "Nack" << std::endl;
    }

    void onTimeout()
    {
        std::cout << "timeout" << std::endl;
    }

    void
    restart()
    {
        std::cout << "Start to wait 5s..." << std::endl;
        m_scheduler.schedule(5_s, bind(&Controller::after10sec, this));
    }

private:
    boost::asio::io_service m_ioService;
    Face m_face;
    Scheduler m_scheduler;
    KeyChain m_keyChain;
};

int
main(int argc, char *argv[])
{
    Controller app;
    try {
        app.run();
    }
    catch (const std::exception &e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
    return 0;
}
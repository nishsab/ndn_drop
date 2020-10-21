//
// Created by NIshant Sabharwal on 10/9/20.
//

#ifndef NDN_DROP_CONFIG_H
#define NDN_DROP_CONFIG_H

#include <ndn-cxx/util/scheduler.hpp>

using namespace ndn;

class Config {
public:
    const static int heartbeatWindow = 5;
};

#endif //NDN_DROP_CONFIG_H

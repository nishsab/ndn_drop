//
// Created by NIshant Sabharwal on 10/25/20.
//

#ifndef NDN_DROP_IDENTITYNAME_H
#define NDN_DROP_IDENTITYNAME_H

#include <ndn-cxx/name.hpp>

class IdentityName {
public:
    static ndn::Name getIdentityName(std::string home, std::string node);
};


#endif //NDN_DROP_IDENTITYNAME_H

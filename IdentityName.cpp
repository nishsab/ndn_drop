//
// Created by NIshant Sabharwal on 10/25/20.
//

#include "IdentityName.h"

using namespace ndn;

Name IdentityName::getIdentityName(std::string home, std::string node) {
    Name name("/ndn/drop/");
    name.append(home);
    name.append(node);
    return name;
}
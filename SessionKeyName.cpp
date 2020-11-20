//
// Created by NIshant Sabharwal on 11/7/20.
//

#include "SessionKeyName.h"

Name SessionKeyName::getSessionKeyName(string home, string node, string keyName) {
    Name name = Name("/ndn/drop");
    name.append(home);
    name.append(node);
    name.append(sess_key);
    name.append(keyName);
    return name;
}

bool SessionKeyName::isSessionKeyName(Name sessionKeyName) {
    return sessionKeyName.size() == MAX && sessionKeyName.get(SESS_KEY).toUri().compare(sess_key) == 0;
}
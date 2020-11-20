//
// Created by NIshant Sabharwal on 11/7/20.
//

#ifndef NDN_DROP_SESSIONKEYNAME_H
#define NDN_DROP_SESSIONKEYNAME_H

#include <ndn-cxx/name.hpp>

using namespace std;
using namespace ndn;

enum SessionKeyNameParts
{
    NDN = 0,
    DROP,
    HOME,
    OWNER,
    SESS_KEY,
    KEY_NAME,
    MAX
};
const string sess_key = "SESS_KEY";

// "ndn/drop/nishant/laptop/SESS_KEY/session_key1"
class SessionKeyName {
public:
    static Name getSessionKeyName(string home, string node, string keyName);
    static bool isSessionKeyName(Name sessionKeyName);
private:
};


#endif //NDN_DROP_SESSIONKEYNAME_H

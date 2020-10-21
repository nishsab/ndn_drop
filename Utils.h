//
// Created by NIshant Sabharwal on 10/7/20.
//

#ifndef NDN_DROP_UTILS_H
#define NDN_DROP_UTILS_H

#include <string>

using namespace std;

class Utils {
public:
    static vector<std::string> split (const string &s, char delim);
    static void logf(const char* format, ...);
    static void errf(const char* format, ...);
};


#endif //NDN_DROP_UTILS_H

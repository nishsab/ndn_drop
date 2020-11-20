//
// Created by NIshant Sabharwal on 10/21/20.
//

#ifndef NDN_DROP_CONF_H
#define NDN_DROP_CONF_H

#include <string>
#include <unordered_map>

using namespace std;
class Conf {
public:
    Conf(string filename);
    int heartbeatWindow;
    string outboundDirectory;
    string inboundDirectory;
    string fileListLocation;
    string schemaConfPath;
    string homeCertificateName;
    string nacIdentityName;
    string nacDataName;
    string nacAccessPrefix;
    string nacCkPrefix;
    int blockSize;
    string repoHostName;
    int repoPort;
    string filePrefix;
    string tpmLocator;
    string pibLocator;
private:
    string getString(unordered_map<string, string> configs, string key);
};


#endif //NDN_DROP_CONF_H

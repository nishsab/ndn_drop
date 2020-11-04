//
// Created by NIshant Sabharwal on 10/21/20.
//

#include <fstream>
#include <iostream>
#include "Conf.h"
#include "Utils.h"
#include <vector>
#include <unordered_map>

Conf::Conf(string filename) {
    ifstream fin(filename);
    string line;
    unordered_map<string, string> configs;
    while (getline(fin, line)) {
        vector<string> tokens = Utils::split(line, '=');
        if (tokens.size() != 2) {
            Utils::errf("Invalid config line: %s\n", line.c_str());
            throw -1;
        }
        configs[tokens[0]] = tokens[1];
    }

    heartbeatWindow = atoi(getString(configs, "heartbeatWindow").c_str());
    outboundDirectory = getString(configs, "outboundDirectory");
    inboundDirectory = getString(configs, "inboundDirectory");
    fileListLocation = getString(configs, "fileListLocation");
}

string Conf::getString(unordered_map<string, string> configs, string key) {
    Utils::errf("Parsing: %s\n", key.c_str());
    string val = configs[key];
    Utils::errf("Setting %s to %s\n", key.c_str(), val.c_str());
    return val;
}
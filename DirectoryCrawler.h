//
// Created by NIshant Sabharwal on 10/22/20.
//

#ifndef NDN_DROP_DIRECTORYCRAWLER_H
#define NDN_DROP_DIRECTORYCRAWLER_H

#include <string>

using namespace std;

class DirectoryCrawler {
public:
    DirectoryCrawler(string directoryPath);
    string getFiles();
private:
    string directoryPath;
};


#endif //NDN_DROP_DIRECTORYCRAWLER_H

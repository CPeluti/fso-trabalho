#ifndef HARDWARE_H
#define HARDWARE_H

#include "cpplibraries.h"
#include "IOManager.h"

class Hardware {
    public:
        static std::vector<std::pair<std::string, int>> disk;
        static std::vector<int> memory;
        static IO IOmanager;
};

#endif
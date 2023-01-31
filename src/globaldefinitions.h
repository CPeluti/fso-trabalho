#ifndef GLOBAL_DEFINITIONS
#define GLOBAL_DEFINITIONS

#include "cpplibraries.h"
#include "process.h"

inline std::vector<std::string> split (std::string s, std::string separator) {
    std::vector<std::string> res;
    std::regex r(separator);
    std::regex_token_iterator<std::string::iterator> regex_end;
    std::regex_token_iterator<std::string::iterator> splitted(s.begin(), s.end(), r, -1);
    while (splitted != regex_end)
        res.push_back(*splitted++);
    return res;
}

#endif
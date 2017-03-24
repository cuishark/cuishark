
#pragma once

#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <sstream>


namespace slankdev {


template <class... ARGS>
static inline std::string strfmt(const char* fmt, ARGS... args)
{
    char str[1000];
    sprintf(str, fmt, args...);
    return str;
}


template <class... ARGS>
static inline std::string fs(const char* fmt, ARGS... args)
{
    char str[1000];
    sprintf(str, fmt, args...);
    return str;
}


static inline std::vector<std::string> split(const std::string &str, char sep)
{
    std::vector<std::string> v;
    std::stringstream ss(str);
    std::string buffer;
    while( std::getline(ss, buffer, sep) ) {
        if (buffer.empty()) continue;
        v.push_back(buffer);
    }
    return v;
}




} /* namespace slankdev */

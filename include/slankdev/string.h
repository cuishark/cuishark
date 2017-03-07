
#pragma once

#include <stdio.h>
#include <string.h>
#include <string>


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


} /* namespace slankdev */

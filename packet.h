
#pragma once

#include <slankdev/exception.h>

class Packet {
public:
    uint8_t buf[2000];
    size_t   len;
    uint64_t time;
    size_t   number;

    Packet(const void* p, size_t l, uint64_t t, size_t n) :
        len(l), time(t), number(n)
    {
        if (l > 2000)
            throw slankdev::exception("FAAAAAAA");
        memcpy(buf, p, len);
    }
    std::string line()
    {
        using namespace slankdev;
        std::string source;
        std::string dest;
        std::string proto;

        ether* eth = reinterpret_cast<ether*>(buf);

        char str[1000];
        sprintf(str, "%5zd %-13ld %-20s %-20s %6s %5zd %-10s" , number, time,
                eth->src.to_string().c_str(),
                eth->dst.to_string().c_str(),
                "protocol", len,
                "yukari is realy realy awesome");
        return str;
    }
};



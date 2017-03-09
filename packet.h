
#pragma once

#include <string.h>
#include <string>
#include <vector>

#include <slankdev/exception.h>
#include "protocol.h"

class Packet {
public:
    std::vector<uint8_t> buf;
    size_t   len;
    uint64_t time;
    size_t   number;
    std::string src;
    std::string dst;
    std::string protocol;
    std::string msg;

    std::vector<Protoblock*> detail_lines;

    Packet(const void* p, size_t l, uint64_t t, size_t n) :
        len(l), time(t), number(n)
    {
        buf.resize(len);
        memcpy(buf.data(), p, len);
        analyze(buf.data(), len);
    }
    virtual ~Packet()
    {
        for (Protoblock* p : detail_lines) {
            delete p;
        }
    }
    void analyze(const uint8_t* ptr, size_t len);
    std::string line()
    {
        using namespace slankdev;
        char str[1000];
        sprintf(str, "%5zd %-13ld %-20s %-20s %6s %5zd %-10s" , number, time,
                src.c_str(),
                dst.c_str(),
                protocol.c_str(), len,
                "yukari is realy realy awesome");
        return str;
    }
};


void Packet::analyze(const uint8_t* ptr, size_t len)
{
    using namespace slankdev;
    detail_lines.clear();

    Ethernet* eth = new Ethernet(ptr, len);
    detail_lines.push_back(eth);
    len -= eth->headerlen();
    ptr += eth->headerlen();
    uint16_t type = eth->type();
    src = eth->src();
    dst = eth->dst();
    protocol = "Ethernet";

    switch (type) {
        case 0x800:
        {
            IP* ip = new IP(ptr, len);
            detail_lines.push_back(ip);
            len -= ip->headerlen();
            ptr += ip->headerlen();
            uint8_t proto = ip->protocol();
            src = ip->src();
            dst = ip->dst();
            protocol = "IPv4";

            switch (proto) {
                case 1:
                {
                    ICMP* icmp = new ICMP(ptr, len);
                    detail_lines.push_back(icmp);
                    len -= icmp->headerlen();
                    ptr += icmp->headerlen();
                    protocol = "ICMP";
                    break;
                }
                case 17:
                {
                    UDP* udp = new UDP(ptr, len);
                    detail_lines.push_back(udp);
                    len -= udp->headerlen();
                    ptr += udp->headerlen();
                    protocol = "UDP";
                    break;
                }
                case 6:
                {
                    TCP* tcp = new TCP(ptr, len);
                    detail_lines.push_back(tcp);
                    len -= tcp->headerlen();
                    ptr += tcp->headerlen();
                    protocol = "TCP";
                    break;
                }
            }
            break;
        }
        case 0x86dd:
        {
            /*
             * Analyze IPv6
             */
            protocol = "IPv6";
            break;
        }
        case 0x0806:
        {
            /*
             * Analyze ARP
             */
            ARP* arp = new ARP(ptr, len);
            detail_lines.push_back(arp);
            len -= arp->headerlen();
            ptr += arp->headerlen();
            protocol = "ARP";
            break;
        }
    }
    if (len > 0) detail_lines.push_back(new Binary(ptr, len));
}

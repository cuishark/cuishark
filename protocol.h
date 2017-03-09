
#pragma once

#include <algorithm>

#include <slankdev/net/protocol.h>
#include <slankdev/string.h>

#include "pane.h"


class line {
public:
    virtual ~line() {}
    virtual std::string to_string() = 0;
};


class staticline : public line {
    const std::string msg;
public:
    staticline(const std::string& str) : msg(str) {}
    std::string to_string() override { return msg; }
};



enum State {
    OPEN,
    CLOSE,
};

class Protoblock : public line {
    State state;
public:
    std::vector<std::string> childs;
    State get_state() const { return state; }

    Protoblock() : state(CLOSE) {}
    void openclose()
    {
        switch (state) {
            case OPEN:
                state = CLOSE;
                break;
            case CLOSE:
                state = OPEN;
                break;
            default:
                throw slankdev::exception("UNKNOWN state");
        }
    }
};




class Ethernet : public Protoblock {
    const slankdev::ether* hdr;
    std::string msg;
public:
    Ethernet(const void* ptr, size_t len) :
        hdr(reinterpret_cast<const slankdev::ether*>(ptr))
    {
        if (len < sizeof(slankdev::ether))
            throw slankdev::exception("length is too small");

        using namespace slankdev;
        childs.push_back(fs("Source       : %s", hdr->dst.to_string().c_str()));
        childs.push_back(fs("Destination  : %s", hdr->src.to_string().c_str()));
        childs.push_back(fs("type         : 0x%04x", htons(hdr->type))        );

        msg = fs("Ethernet, Src: %s, Dst: %s",
            hdr->src.to_string().c_str(), hdr->dst.to_string().c_str());
    }
    std::string src() { return hdr->src.to_string(); }
    std::string dst() { return hdr->dst.to_string(); }
    std::string to_string() { return msg; }
    size_t headerlen() { return sizeof(slankdev::ether); }
    uint16_t type() { return ntohs(hdr->type); }
};

class IP : public Protoblock {
    const slankdev::ip* hdr;
public:
    IP(const void* ptr, size_t len) :
        hdr(reinterpret_cast<const slankdev::ip*>(ptr))
    {
        if (len < sizeof(slankdev::ip))
            throw slankdev::exception("length is too small");
        using namespace slankdev;
        childs.push_back(fs("version          : %u", hdr->ver)                     );
        childs.push_back(fs("header length    : %u(%u) ", hdr->ihl, hdr->ihl<<2)   );
        childs.push_back(fs("Type of service  : 0x%02x ", hdr->tos)                );
        childs.push_back(fs("Data length      : %u  ", ntohs(hdr->len))            );
        childs.push_back(fs("Identifer        : 0x%04x", ntohs(hdr->id ))          );
        childs.push_back(fs("Offset           : 0x%04x", ntohs(hdr->off))          );
        childs.push_back(fs("Time To Leave    : %u (0x%x) ", hdr->ttl, hdr->ttl)   );
        childs.push_back(fs("Protocol         : 0x%x " , hdr->proto, hdr->proto)   );
        childs.push_back(fs("Checksum         : 0x%04x ", ntohs(hdr->sum))         );
        childs.push_back(fs("Source           : %s", hdr->src.to_string().c_str()) );
        childs.push_back(fs("Destination      : %s", hdr->dst.to_string().c_str()) );
    }
    std::string src() { return hdr->src.to_string(); }
    std::string dst() { return hdr->dst.to_string(); }
    std::string to_string() { return "Internet Protocol version 4"; }
    size_t headerlen() { return hdr->ihl << 2; }
    uint8_t protocol() { return hdr->proto; }
};


class ARP : public Protoblock {
    const slankdev::arp* hdr;
    std::string msg;
public:
    ARP(const void* ptr, size_t len) :
        hdr(reinterpret_cast<const slankdev::arp*>(ptr))
    {
        if (len < sizeof(slankdev::arp)) {
            throw slankdev::exception("arp length is too small");
        }
        using namespace slankdev;
        childs.push_back(fs("Hardware type   : 0x%04x", hdr->hwtype)                );
        childs.push_back(fs("Hardware len    : %d", hdr->hwlen )                    );
        childs.push_back(fs("Proto type      : 0x%04x", hdr->ptype )                );
        childs.push_back(fs("Proto len       : %d", hdr->plen  )                    );
        childs.push_back(fs("Operation       : %d", hdr->operation)                 );
        childs.push_back(fs("Source Hardware : %s", hdr->hwsrc.to_string().c_str()) );
        childs.push_back(fs("Target Hardware : %s", hdr->hwdst.to_string().c_str()) );
        childs.push_back(fs("Source Protocol : %s", hdr->psrc.to_string().c_str())  );
        childs.push_back(fs("Target Protocol : %s", hdr->pdst.to_string().c_str())  );

        uint16_t op = ntohs(hdr->operation);
        if (op == 1)      msg = "Address Resolution Protocol (Request)";
        else if (op == 2) msg = "Address Resolution Protocol (Replay)" ;
        else throw slankdev::exception("Unsupport arp op");
    }
    std::string to_string() { return msg; }
    size_t headerlen() { return sizeof(slankdev::arp); }
};


#if 0 // not support
class IPv6 : public Protoblock {
public:
    IPv6(const void* ptr, size_t len)
    {
        if (len < sizeof(slankdev::))
            throw slankdev::exception("length is too small");
        using namespace slankdev;
        childs.push_back("src address"));
        childs.push_back("dst address"));
        childs.push_back("protocol"));
    }
    std::string to_string() { return "Internet Protocol version 6"; }
};
#endif


class ICMP : public Protoblock {
    const slankdev::icmp* hdr;
public:
    ICMP(const void* ptr, size_t len) :
        hdr(reinterpret_cast<const slankdev::icmp*>(ptr))
    {
        if (len < sizeof(slankdev::tcp))
            throw slankdev::exception("length is too small");
        using namespace slankdev;
        childs.push_back(fs("type   : 0x%04x", ntohs(hdr->type))     );
        childs.push_back(fs("code   : 0x%04x", ntohs(hdr->code))     );
        childs.push_back(fs("cksum  : 0x%04x", ntohs(hdr->checksum)) );
    }
    std::string to_string() { return "Internet Control Messaging Protocol"; }
    size_t headerlen() { return sizeof(slankdev::icmp); }
};


class TCP : public Protoblock {
    const slankdev::tcp* hdr;
public:
    TCP(const void* ptr, size_t len) :
        hdr(reinterpret_cast<const slankdev::tcp*>(ptr))
    {
        if (len < sizeof(slankdev::tcp))
            throw slankdev::exception("length is too small");
        using namespace slankdev;
        uint16_t sp = ntohs(hdr->sport);
        uint16_t dp = ntohs(hdr->dport);
        uint32_t s = ntohl(hdr->seq_num);
        uint32_t a = ntohl(hdr->ack_num);
        childs.push_back( fs("Source Port : %u ", sp, sp).c_str()                    );
        childs.push_back( fs("Dest port   : %u ", dp, dp).c_str()                    );
        childs.push_back( fs("Seq number  : %u 0x%08x ", s, s).c_str()               );
        childs.push_back( fs("Ack number  : %u 0x%08x ", a, a).c_str()               );
        childs.push_back( fs("Data offset : 0x%02x    ", hdr->data_off ).c_str()     );
        childs.push_back( fs("Flags       : 0x%02x    ", hdr->tcp_flags).c_str()     );
        childs.push_back( fs("rx win      : 0x%04x    ", ntohs(hdr->rx_win)).c_str() );
        childs.push_back( fs("cksum       : 0x%04x    ", ntohs(hdr->cksum )).c_str() );
    }
    std::string to_string() { return "Transration Control Protocol"; }
    size_t headerlen() { return hdr->data_off; }
};


class UDP : public Protoblock {
    const slankdev::udp* hdr;
public:
    UDP(const void* ptr, size_t len) :
        hdr(reinterpret_cast<const slankdev::udp*>(ptr))
    {
        if (len < sizeof(slankdev::udp))
            throw slankdev::exception("length is too small");
        using namespace slankdev;
        childs.push_back( fs("Source port      : %u\n", ntohs(hdr->src  )).c_str() );
        childs.push_back( fs("Destination port : %u\n", ntohs(hdr->dst  )).c_str() );
        childs.push_back( fs("Data length      : %u\n", ntohs(hdr->len  )).c_str() );
        childs.push_back( fs("Checksum         : %u\n", ntohs(hdr->cksum)).c_str() );
    }
    std::string to_string() { return "User Datagram Protocol"; }
    size_t headerlen() { return sizeof(slankdev::udp); }
};



class Binary : public Protoblock {
    const void* ptr;
    size_t len;
public:
    Binary(const void* p, size_t l) : ptr(p), len(l)
    {
        using namespace slankdev;

        const void* buffer = ptr;
        ssize_t bufferlen   = len;

        const uint8_t *data = reinterpret_cast<const uint8_t*>(buffer);
        size_t row = 0;
        std::string line;

        while (bufferlen > 0) {
            line.clear();

            line += fs("     ");
            size_t n = std::min(bufferlen, ssize_t(16));

            for (size_t i = 0; i < n; i++) {
                if (i == 8) { line += " "; }
                line += fs(" %02x", data[i]);
            }
            for (size_t i = n; i < 16; i++) { line += "   "; }
            line += "   ";
            for (size_t i = 0; i < n; i++) {
                if (i == 8) { line += "  "; }
                uint8_t c = data[i];
            }
            childs.push_back(line.c_str());
            bufferlen -= n;
            data += n;
            row  += n;
        }
    }
    std::string to_string() { return "Binary Data"; }
};




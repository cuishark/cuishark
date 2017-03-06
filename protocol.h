
#pragma once
#include <slankdev/string.h>

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
        childs.push_back(new staticline(
            strfmt("Source       : %s", hdr->dst.to_string().c_str())));
        childs.push_back(new staticline(
            strfmt("Destination  : %s", hdr->src.to_string().c_str())));
        childs.push_back(new staticline(
            strfmt("type         : 0x%04x", htons(hdr->type))));

        msg = strfmt("Ethernet, Src: %s, Dst: %s",
            hdr->src.to_string().c_str(), hdr->dst.to_string().c_str());
    }
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
        childs.push_back(new staticline(
            fs("version          : %u", hdr->ver       ).c_str()          ));
        childs.push_back(new staticline(
            fs("header length    : %u(%u) ", hdr->ihl, hdr->ihl<<2  ).c_str() ));
        childs.push_back(new staticline(
            fs("Type of service  : 0x%02x ", hdr->tos       ).c_str() ));
        childs.push_back(new staticline(
            fs("Data length      : %u  ", ntohs(hdr->len)).c_str() ));
        childs.push_back(new staticline(
            fs("Identifer        : 0x%04x", ntohs(hdr->id )).c_str() ));
        childs.push_back(new staticline(
            fs("Offset           : 0x%04x", ntohs(hdr->off)).c_str() ));
        childs.push_back(new staticline(
            fs("Time To Leave    : %u (0x%x) ", hdr->ttl  , hdr->ttl  ).c_str() ));
        childs.push_back(new staticline(
            fs("Protocol         : %u (0x%x) ", hdr->proto, hdr->proto).c_str() ));
        childs.push_back(new staticline(
            fs("Checksum         : 0x%04x ", ntohs(hdr->sum )).c_str() ));
        childs.push_back(new staticline(
            fs("Source           : %s", hdr->src.to_string().c_str() ).c_str() ));
        childs.push_back(new staticline(
            fs("Destination      : %s", hdr->dst.to_string().c_str() ).c_str() ));
    }
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
        childs.push_back(new staticline(
            fs("Hardware type   : 0x%04x", hdr->hwtype) ));
        childs.push_back(new staticline(
            fs("Hardware len    : %d", hdr->hwlen ) ));
        childs.push_back(new staticline(
            fs("Proto type      : 0x%04x", hdr->ptype ) ));
        childs.push_back(new staticline(
            fs("Proto len       : %d", hdr->plen  ) ));
        childs.push_back(new staticline(
            fs("Operation       : %d", hdr->operation) ));
        childs.push_back(new staticline(
            fs("Source Hardware : %s", hdr->hwsrc.to_string().c_str())));
        childs.push_back(new staticline(
            fs("Target Hardware : %s", hdr->hwdst.to_string().c_str())));
        childs.push_back(new staticline(
            fs("Source Protocol : %s", hdr->psrc.to_string().c_str())));
        childs.push_back(new staticline(
            fs("Target Protocol : %s", hdr->pdst.to_string().c_str())));

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
        childs.push_back(new staticline("src address"));
        childs.push_back(new staticline("dst address"));
        childs.push_back(new staticline("protocol"));
        throw slankdev::exception("IPV6 is not support yet.");
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
        childs.push_back(new staticline("src port"));
        childs.push_back(new staticline("dst port"));
        childs.push_back(new staticline("sequence number"));
        childs.push_back(new staticline("acknoledge number"));
        childs.push_back(new staticline("window size"));
        childs.push_back(new staticline("flags"));
        childs.push_back(new staticline("checksum"));
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
        childs.push_back(new staticline(
            fs("Source Port        : %u 0x%04x ", ntohs(hdr->sport)  , ntohs(hdr->sport)  ).c_str() ));
        childs.push_back(new staticline(
            fs("Destination port   : %u 0x%04x ", ntohs(hdr->dport)  , ntohs(hdr->dport)  ).c_str() ));
        childs.push_back(new staticline(
            fs("Sequence number    : %u 0x%08x ", ntohl(hdr->seq_num), ntohl(hdr->seq_num)).c_str() ));
        childs.push_back(new staticline(
            fs("Acknowledge number : %u 0x%08x ", ntohl(hdr->ack_num), ntohl(hdr->ack_num)).c_str() ));
        childs.push_back(new staticline(
            fs("Data offset        : 0x%02x    ", hdr->data_off                           ).c_str() ));
        childs.push_back(new staticline(
            fs("Flags              : 0x%02x    ", hdr->tcp_flags                          ).c_str() ));
        childs.push_back(new staticline(
            fs("rx win             : 0x%04x    ", ntohs(hdr->rx_win)                      ).c_str() ));
        childs.push_back(new staticline(
            fs("cksum              : 0x%04x    ", ntohs(hdr->cksum )                      ).c_str() ));
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
        childs.push_back(new staticline(
            fs("Source port      : %u\n", ntohs(hdr->src  )).c_str() ));
        childs.push_back(new staticline(
            fs("Destination port : %u\n", ntohs(hdr->dst  )).c_str() ));
        childs.push_back(new staticline(
            fs("Data length      : %u\n", ntohs(hdr->len  )).c_str() ));
        childs.push_back(new staticline(
            fs("Checksum         : %u\n", ntohs(hdr->cksum)).c_str() ));
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
        size_t bufferlen   = len;
        char str[1000];

        const uint8_t *data = reinterpret_cast<const uint8_t*>(buffer);
        size_t row = 0;
        std::string line;

        while (bufferlen > 0) {
            line.clear();

            sprintf(str, "    ");
            line += str;

            size_t n;
            if (bufferlen < 16) n = bufferlen;
            else                n = 16;

            for (size_t i = 0; i < n; i++) {
                if (i == 8) { line += " "; }
                sprintf(str, " %02x", data[i]);
                line += str;
            }
            for (size_t i = n; i < 16; i++) { line += "   "; }
            line += "   ";
            for (size_t i = 0; i < n; i++) {
                if (i == 8) { line += "  "; }
                uint8_t c = data[i];
            }
            childs.push_back(new staticline(line.c_str()));
            bufferlen -= n;
            data += n;
            row  += n;
        }
    }
    std::string to_string() { return "Binary Data"; }
};




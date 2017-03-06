
#pragma once


class Ethernet : public Protoblock {
    const slankdev::ether* hdr;
public:
    Ethernet(const void* ptr, size_t len) :
        hdr(reinterpret_cast<const slankdev::ether*>(ptr))
    {
        if (len < sizeof(slankdev::ether))
            throw slankdev::exception("length is too small");
        childs.push_back(new staticline("dst"));
        childs.push_back(new staticline("src"));
        childs.push_back(new staticline("type"));
    }
    std::string to_string() { return "Ethernet"; }
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
        childs.push_back(new staticline("src address"));
        childs.push_back(new staticline("dst address"));
        childs.push_back(new staticline("protocol"));
    }
    std::string to_string() { return "Internet Protocol version 4"; }
    size_t headerlen() { return hdr->ihl << 2; }
    uint8_t protocol() { return hdr->proto; }
};


class ARP : public Protoblock {
    const slankdev::arp* hdr;
public:
    ARP(const void* ptr, size_t len) :
        hdr(reinterpret_cast<const slankdev::arp*>(ptr))
    {
        if (len < sizeof(slankdev::arp))
            throw slankdev::exception("length is too small");
        using namespace slankdev;
        childs.push_back(new staticline("Operation"));
        childs.push_back(new staticline("Source Hw address"));
        childs.push_back(new staticline("Target Hw address"));
        childs.push_back(new staticline("Source Proto address"));
        childs.push_back(new staticline("Target Proto address"));
    }
    std::string to_string() { return "Adress Resolution Protocol"; }
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
    std::string to_string() { return "Transration Control Protocol"; }
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
        childs.push_back(new staticline("src port"));
        childs.push_back(new staticline("dst port"));
        childs.push_back(new staticline("sequence number"));
        childs.push_back(new staticline("acknoledge number"));
        childs.push_back(new staticline("window size"));
        childs.push_back(new staticline("flags"));
        childs.push_back(new staticline("checksum"));
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
        childs.push_back(new staticline("src port"));
        childs.push_back(new staticline("dst port"));
        childs.push_back(new staticline("length"));
        childs.push_back(new staticline("checksum"));
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
        childs.push_back(new staticline("raw data"));
    }
    std::string to_string() { return "Binary Data"; }
};




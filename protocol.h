
#pragma once

#include <algorithm>

#include <slankdev/net_header.h>
#include <slankdev/string.h>

#include "ToggleListPane.h"



class Ethernet : public ToggleList_Element {
    const slankdev::ether* hdr;
    std::string msg;
    static bool proto_is_close_;
public:
    Ethernet(const void* ptr, size_t len) :
        hdr(reinterpret_cast<const slankdev::ether*>(ptr))
    {
        if (len < sizeof(slankdev::ether))
            throw slankdev::exception("length is too small");

        using namespace slankdev;
        lines.push_back(fs("Source       : %s", hdr->dst.to_string().c_str()));
        lines.push_back(fs("Destination  : %s", hdr->src.to_string().c_str()));
        lines.push_back(fs("type         : 0x%04x", htons(hdr->type))        );

        msg = fs("Ethernet, Src: %s, Dst: %s",
            hdr->src.to_string().c_str(), hdr->dst.to_string().c_str());
    }
    std::string src() { return hdr->src.to_string(); }
    std::string dst() { return hdr->dst.to_string(); }
    size_t headerlen() { return sizeof(slankdev::ether); }
    uint16_t type() { return ntohs(hdr->type); }
    virtual std::string to_string() const override { return msg; }
    virtual void toggle() override
    {
      if (proto_is_close_) proto_is_close_ = false;
      else                 proto_is_close_ = true;
    }
    virtual bool is_close() const override { return proto_is_close_; }
};



class IP : public ToggleList_Element {
    const slankdev::ip* hdr;
    static bool proto_is_close_;
public:
    IP(const void* ptr, size_t len) :
        hdr(reinterpret_cast<const slankdev::ip*>(ptr))
    {
        if (len < sizeof(slankdev::ip))
            throw slankdev::exception("length is too small");
        using namespace slankdev;
        lines.push_back(fs("version          : %u", hdr->ver)                     );
        lines.push_back(fs("header length    : %u(%u) ", hdr->ihl, hdr->ihl<<2)   );
        lines.push_back(fs("Type of service  : 0x%02x ", hdr->tos)                );
        lines.push_back(fs("Data length      : %u  ", ntohs(hdr->len))            );
        lines.push_back(fs("Identifer        : 0x%04x", ntohs(hdr->id ))          );
        lines.push_back(fs("Offset           : 0x%04x", ntohs(hdr->off))          );
        lines.push_back(fs("Time To Leave    : %u (0x%x) ", hdr->ttl, hdr->ttl)   );
        lines.push_back(fs("Protocol         : 0x%x " , hdr->proto, hdr->proto)   );
        lines.push_back(fs("Checksum         : 0x%04x ", ntohs(hdr->sum))         );
        lines.push_back(fs("Source           : %s", hdr->src.to_string().c_str()) );
        lines.push_back(fs("Destination      : %s", hdr->dst.to_string().c_str()) );
    }
    std::string src() { return hdr->src.to_string(); }
    std::string dst() { return hdr->dst.to_string(); }
    size_t headerlen() { return hdr->ihl << 2; }
    uint8_t protocol() { return hdr->proto; }
    virtual std::string to_string() const override { return "Internet Protocol version 4"; }
    virtual void toggle() override
    {
      if (proto_is_close_) proto_is_close_ = false;
      else                 proto_is_close_ = true;
    }
    virtual bool is_close() const override { return proto_is_close_; }
};


class ARP : public ToggleList_Element {
    const slankdev::arp* hdr;
    std::string msg;
    static bool proto_is_close_;
public:
    ARP(const void* ptr, size_t len) :
        hdr(reinterpret_cast<const slankdev::arp*>(ptr))
    {
        if (len < sizeof(slankdev::arp)) {
            throw slankdev::exception("arp length is too small");
        }
        using namespace slankdev;
        uint16_t op = ntohs(hdr->operation);

        lines.push_back(fs("Hardware type   : 0x%04x", ntohs(hdr->hwtype))         );
        lines.push_back(fs("Hardware len    : %d",     hdr->hwlen)                 );
        lines.push_back(fs("Proto type      : 0x%04x", ntohs(hdr->ptype ))         );
        lines.push_back(fs("Proto len       : %d",     hdr->plen)                  );
        lines.push_back(fs("Operation       : %d", op)                             );
        lines.push_back(fs("Source Hardware : %s", hdr->hwsrc.to_string().c_str()) );
        lines.push_back(fs("Target Hardware : %s", hdr->hwdst.to_string().c_str()) );
        lines.push_back(fs("Source Protocol : %s", hdr->psrc.to_string().c_str())  );
        lines.push_back(fs("Target Protocol : %s", hdr->pdst.to_string().c_str())  );

        if (op == 1)      msg = "Address Resolution Protocol (Request)";
        else if (op == 2) msg = "Address Resolution Protocol (Replay)" ;
        else throw slankdev::exception("Unsupport arp op");
    }
    size_t headerlen() { return sizeof(slankdev::arp); }
    virtual std::string to_string() const override { return msg; }
    virtual void toggle() override
    {
      if (proto_is_close_) proto_is_close_ = false;
      else                 proto_is_close_ = true;
    }
    virtual bool is_close() const override { return proto_is_close_; }
};


#if 0 // not support
class IPv6 : public ToggleList_Element {
public:
    IPv6(const void* ptr, size_t len)
    {
        if (len < sizeof(slankdev::))
            throw slankdev::exception("length is too small");
        using namespace slankdev;
        lines.push_back("src address"));
        lines.push_back("dst address"));
        lines.push_back("protocol"));
    }
    std::string to_string() { return "Internet Protocol version 6"; }
};
#endif


class ICMP : public ToggleList_Element {
    const slankdev::icmp* hdr;
    static bool proto_is_close_;
public:
    ICMP(const void* ptr, size_t len) :
        hdr(reinterpret_cast<const slankdev::icmp*>(ptr))
    {
        if (len < sizeof(slankdev::tcp))
            throw slankdev::exception("length is too small");
        using namespace slankdev;
        lines.push_back(fs("type   : 0x%04x", ntohs(hdr->type))     );
        lines.push_back(fs("code   : 0x%04x", ntohs(hdr->code))     );
        lines.push_back(fs("cksum  : 0x%04x", ntohs(hdr->checksum)) );
    }
    uint8_t type() { return hdr->type; }
    uint8_t code() { return hdr->code; }
    size_t headerlen() { return sizeof(slankdev::icmp); }
    virtual std::string to_string() const override { return "Internet Control Messaging Protocol"; }
    virtual void toggle() override
    {
      if (proto_is_close_) proto_is_close_ = false;
      else                 proto_is_close_ = true;
    }
    virtual bool is_close() const override { return proto_is_close_; }
};


class TCP : public ToggleList_Element {
    const slankdev::tcp* hdr;
    static bool proto_is_close_;
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
        lines.push_back( fs("Source Port : %u ", sp, sp).c_str()                    );
        lines.push_back( fs("Dest port   : %u ", dp, dp).c_str()                    );
        lines.push_back( fs("Seq number  : %u 0x%08x ", s, s).c_str()               );
        lines.push_back( fs("Ack number  : %u 0x%08x ", a, a).c_str()               );
        lines.push_back( fs("Data offset : 0x%02x    ", hdr->data_off ).c_str()     );

        uint8_t flags = hdr->tcp_flags;
        std::string str = fs("Flags       : 0x%02x    ", flags);
        if (flags & slankdev::TH_FIN	) str += "F";
        if (flags & slankdev::TH_SYN	) str += "S";
        if (flags & slankdev::TH_RST	) str += "R";
        if (flags & slankdev::TH_PUSH ) str += "P";
        if (flags & slankdev::TH_ACK	) str += "A";
        if (flags & slankdev::TH_URG	) str += "U";

        lines.push_back(str.c_str());
        lines.push_back( fs("rx win      : 0x%04x    ", ntohs(hdr->rx_win)).c_str() );
        lines.push_back( fs("cksum       : 0x%04x    ", ntohs(hdr->cksum )).c_str() );
    }
    uint16_t src() { return ntohs(hdr->sport); }
    uint16_t dst() { return ntohs(hdr->dport); }
    size_t headerlen() { return hdr->data_off; }
    virtual std::string to_string() const override { return "Transration Control Protocol"; }
    virtual void toggle() override
    {
      if (proto_is_close_) proto_is_close_ = false;
      else                 proto_is_close_ = true;
    }
    virtual bool is_close() const override { return proto_is_close_; }
};


class UDP : public ToggleList_Element {
    const slankdev::udp* hdr;
    static bool proto_is_close_;
public:
    UDP(const void* ptr, size_t len) :
        hdr(reinterpret_cast<const slankdev::udp*>(ptr))
    {
        if (len < sizeof(slankdev::udp))
            throw slankdev::exception("length is too small");
        using namespace slankdev;
        lines.push_back( fs("Source port      : %u\n", ntohs(hdr->src  )).c_str() );
        lines.push_back( fs("Destination port : %u\n", ntohs(hdr->dst  )).c_str() );
        lines.push_back( fs("Data length      : %u\n", ntohs(hdr->len  )).c_str() );
        lines.push_back( fs("Checksum         : %u\n", ntohs(hdr->cksum)).c_str() );
    }
    uint16_t src() { return ntohs(hdr->src); }
    uint16_t dst() { return ntohs(hdr->dst); }
    size_t headerlen() { return sizeof(slankdev::udp); }
    virtual std::string to_string() const override { return "User Datagram Protocol"; }
    virtual void toggle() override
    {
      if (proto_is_close_) proto_is_close_ = false;
      else                 proto_is_close_ = true;
    }
    virtual bool is_close() const override { return proto_is_close_; }
};



class Binary : public ToggleList_Element {
    const void* ptr;
    size_t len;
    static bool proto_is_close_;
public:
    Binary(const void* p, size_t l) :
      ptr(p), len(l)
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
            lines.push_back(line.c_str());
            bufferlen -= n;
            data += n;
            row  += n;
        }
    }
    virtual std::string to_string() const override { return "Binary Data"; }
    virtual void toggle() override
    {
      if (proto_is_close_) proto_is_close_ = false;
      else                 proto_is_close_ = true;
    }
    virtual bool is_close() const override { return proto_is_close_; }
};



bool Ethernet::proto_is_close_ = true;
bool IP::proto_is_close_ = true;
bool ICMP::proto_is_close_ = true;
bool ARP::proto_is_close_ = true;
bool TCP::proto_is_close_ = true;
bool UDP::proto_is_close_ = true;
bool Binary::proto_is_close_ = true;

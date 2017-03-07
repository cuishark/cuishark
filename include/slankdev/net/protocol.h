
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string>
#include <slankdev/socketfd.h>



namespace slankdev {




#pragma pack(1)


struct ether_addr {
    uint8_t addr_bytes[6];

    std::string to_string() const
    {
        static char str[256];
        sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
                addr_bytes[0], addr_bytes[1], addr_bytes[2],
                addr_bytes[3], addr_bytes[4], addr_bytes[5]);
        return str;
    }
};


struct ether {
    slankdev::ether_addr dst;
    slankdev::ether_addr src;
    uint16_t type;

    void print() const
    {
        printf("Ether header\n");
        printf("+ dst  : %s    \n", dst.to_string().c_str());
        printf("+ src  : %s    \n", src.to_string().c_str());
        printf("+ type : 0x%04x\n", type);
    }
};



struct in_addr {
    uint32_t s_addr;

    std::string to_string() const
    {
        union {
            uint8_t u8[4];
            uint32_t u32;
        } U;

        U.u32 = s_addr;
        static char str[256];
        sprintf(str, "%d.%d.%d.%d",
                U.u8[0], U.u8[1], U.u8[2], U.u8[3]);
        return str;
    }
};



struct ip {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint8_t  ihl:4    ;     /* header length          */
    uint8_t  ver:4;         /* version                */
#elif __BYTE_ORDER == __BIG_ENDIAN
    uint8_t  ver:4;         /* version                */
    uint8_t  ihl:4    ;     /* header length          */
#else
# error "fix endian!!"
#endif
    uint8_t   tos  ;        /* type of service        */
    uint16_t  len  ;        /* total length           */
    uint16_t  id   ;        /* identification         */
    uint16_t  off  ;        /* fragment offset field  */
    uint8_t   ttl  ;        /* time to live (TTL)     */
    uint8_t   proto;        /* protocol               */
    uint16_t  sum  ;        /* header checksum        */
    slankdev::in_addr  src; /* source ip address      */
    slankdev::in_addr  dst; /* destination ip address */

    void print() const
    {
        printf("IP header\n");
        printf("+ ver  : %u        \n", ver       );
        printf("+ hlen : %u        \n", ihl       );
        printf("+ tos  : 0x%02x    \n", tos       );
        printf("+ len  : %u        \n", ntohs(len));
        printf("+ id   : 0x%04x    \n", ntohs(id ));
        printf("+ off  : 0x%04x    \n", ntohs(off));
        printf("+ ttl  : %u (0x%x) \n", ttl  , ttl  );
        printf("+ proto: %u (0x%x) \n", proto, proto);
        printf("+ sum  : 0x%04x    \n", ntohs(sum ));
        printf("+ src  : %s  \n", src.to_string().c_str());
        printf("+ dst  : %s  \n", dst.to_string().c_str());
    }
};



struct arp {
    uint16_t    hwtype   ;
    uint16_t    ptype    ;
    uint8_t     hwlen    ;
    uint8_t     plen     ;
    uint16_t    operation;
    ether_addr  hwsrc    ;
    in_addr     psrc     ;
    ether_addr  hwdst    ;
    in_addr     pdst     ;

    void print() const
    {
        printf("ARP header\n");
        printf("+ hwtype   : %u\n", ntohs(hwtype   ));
        printf("+ ptype    : %u\n", ntohs(ptype    ));
        printf("+ hwlen    : %u\n", hwlen    );
        printf("+ plen     : %u\n", plen     );
        printf("+ operation: %u\n", ntohs(operation));
        printf("+ hwsrc    : %s\n", hwsrc.to_string().c_str());
        printf("+ psrc     : %s\n", psrc.to_string().c_str());
        printf("+ hwdst    : %s\n", hwdst.to_string().c_str());
        printf("+ pdst     : %s\n", pdst.to_string().c_str());
    }
};




struct udp {
    uint16_t src  ;
    uint16_t dst  ;
    uint16_t len  ;
    uint16_t cksum;

    void print() const
    {
        printf("UDP header\n");
        printf("+ src  : %u\n", ntohs(src  ));
        printf("+ dst  : %u\n", ntohs(dst  ));
        printf("+ len  : %u\n", ntohs(len  ));
        printf("+ cksum: %u\n", ntohs(cksum));
    }
};



struct tcp {
	uint16_t sport;     /* TCP source port.            */
	uint16_t dport;     /* TCP destination port.       */
	uint32_t seq_num;   /* TX data sequence number.    */
	uint32_t ack_num;   /* RX data ack number.         */
	uint8_t  data_off;  /* Data offset.                */
	uint8_t  tcp_flags; /* TCP flags                   */
	uint16_t rx_win;    /* RX flow control window.     */
	uint16_t cksum;     /* TCP checksum.               */
	uint16_t tcp_urp;   /* TCP urgent pointer, if any. */

    void print() const
    {
        printf("TCP header \n");
        printf("+ sport    : %u 0x%04x \n", ntohs(sport)  , ntohs(sport)  );
        printf("+ dport    : %u 0x%04x \n", ntohs(dport)  , ntohs(dport)  );
        printf("+ seq num  : %u 0x%08x \n", ntohl(seq_num), ntohl(seq_num));
        printf("+ ack num  : %u 0x%08x \n", ntohl(ack_num), ntohl(ack_num));
        printf("+ data off : 0x%02x \n", data_off                         );
        printf("+ tcp flags: 0x%02x \n", tcp_flags                        );
        printf("+ rx win   : 0x%04x \n", ntohs(rx_win)                    );
        printf("+ cksum    : 0x%04x \n", ntohs(cksum )                    );
    }
};


struct icmp {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;

    void print() const
    {
        printf("ICMP header \n");
        printf("+ type     : %u \n", type);
        printf("+ code     : %u \n", code);
        printf("+ checksum : %u \n", ntohs(checksum));
    }
};



#pragma pack(0)

} /* namespace slankdev */

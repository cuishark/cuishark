
#pragma once

#include <slankdev/intfd.h>

#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <net/if.h>

#ifdef __linux__
#include <netpacket/packet.h>
#endif

#ifndef ETH_P_ALL
#define ETH_P_ALL  0x0003
#endif
#ifndef ETH_P_IP
#define ETH_P_IP   0x0800
#endif
#ifndef ETH_P_ARP
#define ETH_P_ARP  0x0806
#endif
#ifndef ETH_P_RARP
#define ETH_P_RARP 0x0835
#endif

#ifndef IPPROTO_ICMP
#define IPPROTO_ICMP 1
#endif
#ifndef IPPROTO_TCP
#define IPPROTO_TCP  6
#endif
#ifndef IPPROTO_UDP
#define IPPROTO_UDP  17
#endif
#ifndef IPPROTO_RAW
#define IPPROTO_RAW  255
#endif




uint8_t arp_raw[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x06, 0x00, 0x01,
    0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
    0x00, 0x00 };


namespace slankdev {

void string2binary_in4(const char* src, struct ::sockaddr_in* dst)
{
    int res = inet_pton(AF_INET, src, &dst->sin_addr);
    if (res != 1) {
        perror("inet_pton");
        exit(-1);
    }
}


void binary2string_in4(const struct sockaddr_in* src, char* dst, size_t dstlen)
{
    const char* res = inet_ntop(AF_INET, &src->sin_addr, dst, dstlen);
    if (res == NULL) {
        perror("inet_ntop");
        exit(-1);
    }
}



class socketfd : public safe_intfd {
    public:
        socketfd() {}
        socketfd(int fd)
        {
            set_fd(fd);
        }
        bool dead() const
        {
            return fd < 0;
        }
        void socket(int domain, int type, int protocol)
        {
            fd = ::socket(domain, type, protocol);
            if (fd < 0) {
                perror("socket");
                exit(-1);
            }
        }
        void connect(const struct sockaddr* addr, socklen_t addrlen)
        {
            int res = ::connect(fd, addr, addrlen);
            if (res < 0) {
                perror("connect");
                exit(-1);
            }
        }
        void bind(const struct sockaddr* sa, size_t len)
        {
            int res = ::bind(fd, sa, len);
            if (res < 0) {
                perror("bind");
                exit(-1);
            }
        }
        void listen(int backlog)
        {
            int res = ::listen(fd, backlog);
            if (res < 0) {
                perror("listen");
                exit(-1);
            }
        }
        int  accept(struct sockaddr* sa, socklen_t* len)
        {
            int res = ::accept(fd, sa, len);
            if (res < 0) {
                perror("accept");
                exit(-1);
            }
            return res;
        }
        void sendto(const void* buffer, size_t bufferlen,int flags,
                const struct sockaddr* dest_addr, socklen_t dest_len)
        {
            ssize_t res = ::sendto(fd, buffer, bufferlen, flags, dest_addr, dest_len);
            if (res < 0) {
                perror("sendto");
                exit(-1);
            }
        }
        size_t recvfrom(void* buffer, size_t bufferlen, int flags,
                struct sockaddr* address, socklen_t* address_len)
        {
            ssize_t res = ::recvfrom(fd, buffer, bufferlen, flags, address, address_len);
            if (res < 0) {
                perror("recvfrom");
                exit(-1);
            }
            return res;
        }
        void getsockopt(int level, int optname, void* optval, socklen_t *optlen)
        {
            int res = ::getsockopt(fd, level, optname, optval, optlen);
            if (res < 0) {
                perror("getsockopt");
                exit(-1);
            }
        }
        void setsockopt(int level, int optname, const void* optval, socklen_t optlen)
        {
            int res = ::setsockopt(fd, level, optname, optval, optlen);
            if (res < 0) {
                perror("setsockopt");
                exit(-1);
            }
        }
        void send(const void* buf, size_t nbyte, int flags)
        {
            int res = ::send(fd, buf, nbyte, flags);
            if (res < 0) {
                perror("send");
                exit(-1);
            }
        }
        size_t recv(void* buf, size_t nbyte, int flags)
        {
            ssize_t res = ::recv(fd, buf, nbyte, flags);
            if (res < 0) {
                perror("recv");
                exit(-1);
            }
            return res;
        }

#ifdef __linux__
        void open_if(const char* name)
        {
            socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

            struct ifreq ifreq;
            memset(&ifreq, 0, sizeof(ifreq));
            strncpy(ifreq.ifr_name, name, sizeof(ifreq.ifr_name)-1);
            ioctl(SIOCGIFINDEX, &ifreq);

            struct sockaddr_ll sa;
            sa.sll_family = PF_PACKET;
            sa.sll_protocol = htons(ETH_P_ALL);
            sa.sll_ifindex = ifreq.ifr_ifindex;
            bind((struct sockaddr*)&sa, sizeof(sa));

            ioctl(SIOCGIFFLAGS, &ifreq);
            ifreq.ifr_flags = ifreq.ifr_flags | IFF_PROMISC;
            ioctl(SIOCSIFFLAGS, &ifreq);
        }
#endif
};




} /* namespace slankdev */

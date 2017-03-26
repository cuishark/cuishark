

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <arpa/inet.h>

/* for split */
#include <vector>
#include <string>
#include <sstream>

#define UNUSED(x) (void)(x)

namespace slankdev {


template <class T>
static inline void vec_delete_all_ptr_elements(std::vector<T*>& vec)
{
    while (vec.size() > 0) {
        T* tmp = vec.back();
        vec.pop_back();
        delete tmp;
    }
}

template <class... ARGS>
inline void fdprintf(int fd, const char* fmt, ARGS... args)
{
    FILE* fp = fdopen(fd, "w");
    ::fprintf(fp, fmt, args...);
    fflush(fp);
}


/* Thanks @herumi */
static uint16_t read_as_little_endian(const void* data)
{
    const uint8_t* p = reinterpret_cast<const uint8_t*>(data);
    return uint16_t(p[0]) | (uint16_t(p[1]) << 8);
}
#if 0
/* Thanks @herumi, Isn't used yet */
static uint16_t read_as_big_endian(const void* data)
{
    const uint8_t* p = reinterpret_cast<const uint8_t*>(data);
    return uint16_t(p[1]) | (uint16_t(p[0]) << 8);
}
#endif

inline uint16_t checksum(const void* data, size_t len)
{
    uint32_t sum;
    const uint8_t* data_pointer = reinterpret_cast<const uint8_t*>(data);

    for (; len > 1; len-=2, data_pointer+=2) {
        uint16_t tmp = read_as_little_endian(data_pointer);
        sum += tmp;
    }

    if (len == 1) {
        fprintf(stderr, "pgen::checksum: byte length isn't even num\n");
        // uint8_t tmp = *data_pointer;
        uint16_t tmp = read_as_little_endian(data_pointer);
        sum += tmp;
    }

    uint16_t overflowd = sum >> 16;
    sum = sum & 0x0000ffff;
    sum = sum + overflowd;

    return ~sum;
}



inline void hexdump(const char* title, const void *buffer, size_t bufferlen)
{
    printf("%s at [%p] length=%zd \n", title, buffer, bufferlen);

    const uint8_t *data = reinterpret_cast<const uint8_t*>(buffer);
    size_t row = 0;
    while (bufferlen > 0) {
        printf("%04zx:   ", row);

        size_t n;
        if (bufferlen < 16) n = bufferlen;
        else                n = 16;

        for (size_t i = 0; i < n; i++) {
            if (i == 8) printf(" ");
            printf(" %02x", data[i]);
        }
        for (size_t i = n; i < 16; i++) {
            printf("   ");
        }
        printf("   ");
        for (size_t i = 0; i < n; i++) {
            if (i == 8) printf("  ");
            uint8_t c = data[i];
            if (!(0x20 <= c && c <= 0x7e)) c = '.';
            printf("%c", c);
        }
        printf("\n");
        bufferlen -= n;
        data += n;
    }
}


inline void bindump(const char* title, const void* buffer, size_t bufferlen)
{
    printf("%s at [%p] length=%zd \n", title, buffer, bufferlen);

    const uint8_t* p0 = reinterpret_cast<const uint8_t*>(buffer);
    char buf[] = "00000000 ";
    for (size_t i=0; i<bufferlen; i++) {
        uint8_t c = p0[i];
        for (size_t j = 0; j < 8; j++) {
            buf[j] = '0' + (c & 1);
            c >>= 1;
        }
        printf("%s", buf);
        if ((i+1) % 8 == 0) printf("\n");
    }
}


#if 0
uint16_t htons(uint16_t n)
{
    return ::htons(n);
}
uint16_t ntohs(uint16_t n)
{
    return ::ntohs(n);
}
#endif

inline uint16_t bswap16(uint16_t n)
{
    return ((n << 8)&0xff00) | ((n>>8)&0x00ff);
}

inline uint32_t bswap32(uint32_t n)
{
    return ((n << 24)&0xff000000) | ((n<<8)&0x00ff0000)
        | ((n>>8)&0x0000ff00) | ((n>>24)&0x000000ff);
}


inline void clear_screen()
{
    printf("\033[2J\n");
}



} /* namespace slankdev */

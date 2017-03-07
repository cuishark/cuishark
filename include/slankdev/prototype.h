
#pragma once

#include <stdio.h>
#include <iostream>



namespace slankdev {


#if 0
#define slankdev_bswap_16(n) \
    (((((unsigned short)(n) & 0xFF)) << 8) | (((unsigned short)(n) & 0xFF00) >> 8))

#define slankdev_bswap_32(n) \
    (((((unsigned long)(n) & 0xFF)) << 24)   | ((((unsigned long)(n) & 0xFF00)) << 8) | \
	((((unsigned long)(n) & 0xFF0000)) >> 8) | ((((unsigned long)(n) & 0xFF000000)) >> 24))


#ifdef __linux
# if __BYTE_ORDER == __BIG_ENDIAN
// # warning "BIG ENDIAN"
#  define slankdev_ntohl(x)	x
#  define slankdev_ntohs(x)	x
#  define slankdev_htonl(x)	x
#  define slankdev_htons(x)	x
# elif __BYTE_ORDER == __LITTLE_ENDIAN
// # warning "LITTLE ENDIAN"
#  define slankdev_ntohl(x)	slankdev_bswap_32 (x)
#  define slankdev_ntohs(x)	slankdev_bswap_16 (x)
#  define slankdev_htonl(x)	slankdev_bswap_32 (x)
#  define slankdev_htons(x)	slankdev_bswap_16 (x)
# else
# error "Unknown __BYTE_ORDER"
# endif
#endif
#endif


#if 0
#ifdef __linux
uint16_t htons(uint16_t val)
{
    return slankdev_htons(val);
}
uint16_t ntohs(uint16_t val)
{
    return slankdev_ntohs(val);
}
uint32_t htonl(uint32_t val)
{
    return slankdev_htonl(val);
}
uint32_t ntohl(uint32_t val)
{
    return slankdev_ntohl(val);
}
#endif
#endif


/*
 * This class manages Battery state.
 * However, not implemented yet.
 */
// class battery {
//     private:
//         std::vector<safe_intfd> bats_full;
//         std::vector<safe_intfd> bats_now;
//
//     public:
//         battery();
//         void update_state();
//         int percent();
// };


// Sample Code that Gets Battery State
// #include <stdio.h>
// #include <string.h>
// #include <slankdev.h>
// #define BAT1_FULL "/sys/class/power_supply/BAT1/energy_full"
// #define BAT1_NOW  "/sys/class/power_supply/BAT1/energy_now"
// #define BAT0_FULL "/sys/class/power_supply/BAT0/energy_full"
// #define BAT0_NOW  "/sys/class/power_supply/BAT0/energy_now"
// static const char* getstr(const char* path)
// {
//     slankdev::safe_intfd fd;
//     fd.open(path, O_RDONLY);
//     static char str[256];
//     memset(str, 0, sizeof(str));
//     ssize_t readlen = fd.read(str, sizeof(str));
//     return str;
// }
// void print(const char* fullpath, const char* nowpath)
// {
//     int full = atoi(getstr(fullpath));
//     int now  = atoi(getstr(nowpath));
//     double per  = (double)(now) / full * 100;
//     printf("full: %d\n", full);
//     printf("now : %d\n", now );
//     printf("per : %d\n", (int)per );
// }
// int main()
// {
//     print(BAT1_FULL, BAT1_NOW);
//     print(BAT0_FULL, BAT0_NOW);
// }



#if 0

template <typename T>
class simple_singleton {
protected:
    simple_singleton() {}
    ~simple_singleton() {}
    simple_singleton(const simple_singleton&) = delete;
    simple_singleton& operator=(const simple_singleton&) = delete;

public:
    static T& instance()
    {
        static T instnc;
        return instnc;
    }
};




template <typename T>
class pointer_singleton {
protected:
    static T* point;
    static bool inited;

    pointer_singleton() {}
    pointer_singleton(const pointer_singleton&) = delete;
    pointer_singleton& operator=(const pointer_singleton&) = delete;
public:
    static T* instance()
    {
        if (inited == false) {
            point = instance<>();
        }
        return point;
    }
    template <typename... Arg>
    static T* instance(Arg... args)
    {
        if (inited) {
            fprintf(stderr, "FATAL: reinit... (x.x)\n");
            exit(-1);
        }
        point = new T(args...);
        inited = true;
        return  point;
    }
};
template <typename T>
T* pointer_singleton<T>::point = nullptr;
template <typename T>
bool pointer_singleton<T>::inited = false;




/*
 * TODO
 * This class is unsafe..
 * I couldn't implement cleverly.
 */

#if 0
template <typename T>
class reference_singleton {
protected:
    static bool inited;
    reference_singleton() {}
    reference_singleton(const reference_singleton&) = delete;
    reference_singleton& operator=(const reference_singleton&) = delete;

public:
    static T& instance()
    {
        if (inited == false) {
            T& ref = instance<>();
        }
        static T t;
        return t;
    }
    template <typename... Arg>
    static T& instance(Arg... args)
    {
        T& t = instance();
        if (inited == false) {
            t._constructor(args...);
            inited = true;
        } else {
            printf("try to reinit\n");
            exit(-1);
        }
        return t;
    }
};
template <typename T>
bool reference_singleton<T>::inited = false;
#endif

#endif



#if 0


/*
 * If you want to generate queue-instance.
 * the class that is specified as T must have
 * member variable 'next'.
 */
template <typename T>
class queue {
    private:

        T* get_tail()
        {
            T* node = head;
            if (!node)
                return nullptr;

            while (node->next)
                node = node->next;
            return node;
        }

        T* head;
    public:
        queue() : head(nullptr) {}
        ~queue()
        {

            T* link = head;
            while (link) {
                T* next = link->next;
                free(link)
                link = next;
            }

        }
        bool empty() const
        {
            return (!head);
        }
        size_t size() const
        {
            size_t cnt = 0;
            T* node = head;
            while (node) {
                cnt++;
                node = node->next;
            }
            return cnt;
        }
        T* front() const
        {
            return head;
        }
        void push(T* node) /* insert to tail */
        {
            T* tail = get_tail();
            if (tail) {
                tail->next = node;
            } else {
                head = node;
            }
        }
        T* pop()
        {
            T* node = head;
            if (!node) return nullptr;

            head = node->next;
            node->next = nullptr;
            return node;
        }
};



#endif


} /* namespace slankdev */

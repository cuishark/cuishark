


#include <pcap.h>
#include <stdio.h>
#include <poll.h>

#include <vector>
#include <queue>
#include <algorithm>

#include <slankdev/util.h>
#include <slankdev/ncurses.h>
#include <slankdev/exception.h>
#include <slankdev/net/protocol.h>



class Packet {
public:
    uint8_t* ptr;
    size_t   len;
    uint64_t time;

    Packet(const void* p, size_t l, uint64_t t) : ptr((uint8_t*)p), len(l), time(t) {}
    std::string line(int number)
    {
        using namespace slankdev;
        std::string source;
        std::string dest;
        std::string proto;

        ether* eth = reinterpret_cast<ether*>(ptr);

        char str[1000];
        sprintf(str, "%5d %10ld %-20s %-20s %6s %5zd %-10s" , number, time,
                eth->src.to_string().c_str(),
                eth->dst.to_string().c_str(),
                "protocol", len, "summry");
        return str;
    }
};



class pane {
protected:
    size_t current_x;
    size_t current_y;
    slankdev::ncurses& screen;
public:
    const size_t x;
    const size_t y;
    const size_t w;
    const size_t h;
    pane(size_t ix, size_t iy, size_t iw, size_t ih, slankdev::ncurses& scr) :
        x(ix), y(iy), w(iw), h(ih), current_x(ix), current_y(iy), screen(scr) {}
    void println(const char* str)
    {
        screen.mvprintw(current_y++, current_x, "%s", str);
        current_x = x;
    }
    void println_hl(const char* str)
    {
        attron(A_REVERSE);
        screen.mvprintw(current_y++, current_x, "%s", str);
        current_x = x;
        attroff(A_REVERSE);
    }
    void print(const char* str) { screen.mvprintw(current_y, current_x, "%s", str); }
    void putc(char c)
    {
        if (c == '\n') {
            current_y ++ ;
            current_x = x;
        } else {
            screen.mvprintw(current_y, current_x++, "%c", c);
        }
    }
};




class Pane_list : public pane {
    std::vector<Packet> packets;
public:
    size_t cursor_index;
    Pane_list(size_t ix, size_t iy, size_t iw, size_t ih, slankdev::ncurses& scr) :
        pane(ix, iy, iw, ih, scr), cursor_index(0)
    {
    }
    void push_packet(const void* packet, struct pcap_pkthdr* hdr)
    {
        static int number = 0;
        Packet pack(packet, hdr->len, hdr->ts.tv_sec);
        packets.push_back(pack);
    }
    void refresh()
    {
        current_x = x;
        current_y = y;
        char str[1000];
        sprintf(str, "%-5s %-10s %-20s %-20s %-6s %-5s %-10s",
                "No.", "Time", "Source", "Destination",
                "Protocol", "Len", "Info");
        println(str);

        size_t start_idx = cursor_index;
        for (size_t i=start_idx, c=0; i<packets.size() && c<h; i++, c++) {
            if (i == cursor_index)
                println_hl(packets[i].line(i).c_str());
            else
                println(packets[i].line(i).c_str());
        }
    }
};

class Pane_detail : public pane {
public:
    Pane_detail(size_t ix, size_t iy, size_t iw, size_t ih, slankdev::ncurses& scr) :
        pane(ix, iy, iw, ih, scr) {}
};


class display {
    slankdev::ncurses screen;
    pcap_t *handle;
public:
    Pane_list   pane_list;
    Pane_detail pane_detail;

    display() :
        pane_list(0,0,screen.getw(),screen.geth()/2, screen),
        pane_detail(0, screen.geth()/2, screen.getw(), screen.geth()/2, screen)
    {
        char errbuf[PCAP_ERRBUF_SIZE];
        handle = pcap_open_live("lo", BUFSIZ, 0, 0, errbuf);
        if (handle == NULL) {
            throw slankdev::exception("pcap_open_live");
        }
        noecho();
        screen.refresh();
    }
    ~display() { pcap_close(handle); }
    void dispatch()
    {
        struct pollfd fds[2];
        fds[0].fd = pcap_get_selectable_fd(handle);
        fds[0].events = POLLIN;
        fds[1].fd = fileno(stdin);
        fds[1].events = POLLIN;
        while (1) {
            if (poll(fds, 2, 100)) {

                if (fds[0].revents & POLLIN) {
                    struct pcap_pkthdr header;
                    const u_char *packet;
                    packet = pcap_next(handle, &header);
                    pane_list.push_packet(packet, &header);
                }

                if (fds[1].revents & POLLIN) {
                    int c = screen.getchar();
                    switch (c) {
                        case 'j':
                            if (pane_list.cursor_index + 1 < pane_list.h)
                                pane_list.cursor_index ++;
                            break;
                        case 'k':
                            if (pane_list.cursor_index - 1 > 0)
                            pane_list.cursor_index --;
                            break;
                    }
                    pane_detail.putc(c);
                }
            }
            pane_list.refresh();
            screen.refresh();
        }
    }
};


int main(int argc, char *argv[])
{
    display dsp0;
    dsp0.dispatch();
}



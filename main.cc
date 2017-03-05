


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
    uint8_t buf[2000];
    size_t   len;
    uint64_t time;
    size_t   number;

    Packet(const void* p, size_t l, uint64_t t, size_t n) :
        len(l), time(t), number(n)
    {
        memcpy(buf, p, len);
    }
    std::string line()
    {
        using namespace slankdev;
        std::string source;
        std::string dest;
        std::string proto;

        ether* eth = reinterpret_cast<ether*>(buf);

        char str[1000];
        sprintf(str, "%5zd %-13ld %-20s %-20s %6s %5zd %-10s" , number, time,
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
    template <class... Arg>
    void println(const char* fmt, Arg... arg)
    {
        screen.mvprintw(current_y++, current_x, fmt, arg...);
        current_x = x;
    }
    template <class... Arg>
    void println_hl(const char* fmt, Arg... arg)
    {
        attron(A_REVERSE);
        screen.mvprintw(current_y++, current_x, fmt, arg...);
        current_x = x;
        attroff(A_REVERSE);
    }
    template <class... Arg>
    void print(const char* fmt, Arg... arg)
    {
        char str[1000];
        sprintf(str, fmt, arg...);
        size_t len = strlen(str);
        screen.mvprintw(current_y, current_x+=len, fmt, arg...);
    }
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
    ssize_t cursor_index;
    size_t  list_start_index;
public:
    std::vector<Packet> packets;
    ssize_t get_cursor() { return cursor_index; }
    void dec_cursor()
    {
        if (cursor_index-1 < list_start_index) scroll_up();
        if (get_cursor()-1 >= 0) cursor_index--;
    }
    void inc_cursor()
    {
        if (cursor_index+1-list_start_index >= h) scroll_down();
        if (get_cursor()+1 < packets.size()) cursor_index++;
    }
    void scroll_up()   { list_start_index --; }
    void scroll_down() { list_start_index ++; }
    Pane_list(size_t ix, size_t iy, size_t iw, size_t ih,
            slankdev::ncurses& scr) : pane(ix, iy, iw, ih, scr),
            cursor_index(0), list_start_index(0) {}
    void push_packet(const void* packet, struct pcap_pkthdr* hdr)
    {
        static int number = 0;
        packets.emplace_back(packet, hdr->len, hdr->ts.tv_sec, number++);
    }
    void refresh()
    {
        current_x = x;
        current_y = y;
        println("%-5s %-13s %-20s %-20s %-6s %-5s %-10s",
                "No.", "Time", "Source", "Destination",
                "Protocol", "Len", "Info");

        size_t start_idx = list_start_index;
        for (size_t i=start_idx, c=0; i<packets.size() && c<h; i++, c++) {
            if (i == get_cursor())
                println_hl(packets[i].line().c_str());
            else
                println(packets[i].line().c_str());
        }
    }
};





class Pane_detail : public pane {
public:
    Pane_detail(size_t ix, size_t iy, size_t iw, size_t ih,
            slankdev::ncurses& scr) : pane(ix, iy, iw, ih, scr) {}
    void print_packet_detail(Packet* packet)
    {
        current_x = x;
        current_y = y;
        println("Packet Detail");
        println(" + number : %zd ", packet->number);
        println(" + pointer: %p", packet->buf);
        println(" + length : %zd", packet->len);
        println(" + time   : %ld", packet->time);


    }
    void refresh() {}
};



class Pane_binary : public pane {
public:
    Pane_binary(size_t ix, size_t iy, size_t iw, size_t ih,
            slankdev::ncurses& scr) : pane(ix, iy, iw, ih, scr) {}
    void hex(Packet* packet)
    {
        current_x = x;
        current_y = y;
        const void* buffer = packet->buf;
        size_t bufferlen   = packet->len;
        println("[%p] length=%zd", buffer, bufferlen);

        const uint8_t *data = reinterpret_cast<const uint8_t*>(buffer);
        size_t row = 0;
        while (bufferlen > 0) {
            print("%04zx:   ", row);

            size_t n;
            if (bufferlen < 16) n = bufferlen;
            else                n = 16;

            for (size_t i = 0; i < n; i++) {
                if (i == 8) print(" ");
                print(" %02x", data[i]);
            }
            for (size_t i = n; i < 16; i++) {
                print("   ");
            }
            print("   ");
            for (size_t i = 0; i < n; i++) {
                if (i == 8) print("  ");
                uint8_t c = data[i];
                if (!(0x20 <= c && c <= 0x7e)) c = '.';
                print("%c", c);
            }
            println("");
            bufferlen -= n;
            data += n;
        }
    }
    void refresh() {}
};


enum cursor_state {
    LIST,
    DETAIL,
    BINARY,
};


class Statusline {
    const size_t x;
    const size_t y;
    const size_t w;
    const cursor_state& state;
public:
    Statusline(size_t ix, size_t iy, size_t iw, cursor_state& c)
        : x(ix), y(iy), w(iw), state(c) {}
    const char* state2str(cursor_state s)
    {
        switch (s) {
            case LIST:   return "LIST   ";
            case DETAIL: return "DETAIL ";
            case BINARY: return "BINARY ";
            default :    return "UNKNOWN";
        }
    }
    void refresh()
    {
        mvprintw(y, x, "[CUISHARK] pane=%s", state2str(state));
    }
};

size_t a(size_t h) { return (h-1) % 3; }
size_t m(size_t h) { return ((h-1)-((h-1)%3))/3; }
#define H screen.geth()

class display {
    slankdev::ncurses screen;
    pcap_t *handle;
    cursor_state cstate;
public:
    Pane_list   pane_list;
    Pane_detail pane_detail;
    Pane_binary pane_binary;
    Statusline statusline;

    display() :
        cstate(LIST),
        pane_list  (0, 0          , screen.getw(), a(H)+m(H), screen),
        pane_detail(0, a(H)+m(H)  , screen.getw(), m(H)     , screen),
        pane_binary(0, a(H)+2*m(H), screen.getw(), m(H)     , screen),
        statusline (0, a(H)+3*m(H), screen.getw(), cstate)
    {
        char errbuf[PCAP_ERRBUF_SIZE];
        handle = pcap_open_live("lo", BUFSIZ, 0, 0, errbuf);
        // handle = pcap_open_offline("in.pcap", errbuf);
        if (handle == NULL) {
            throw slankdev::exception("pcap_open_live");
        }
        noecho();
        screen.refresh();
    }
    ~display() { pcap_close(handle); }
    void charnge_cstate()
    {
        switch (cstate) {
            case LIST:
                cstate = DETAIL;
                break;
            case DETAIL:
                cstate = BINARY;
                break;
            case BINARY:
                cstate = LIST;
                break;
            default :
                throw slankdev::exception("UNKNOWN state");
                break;
        }
    }
    void press_j()
    {
        switch (cstate) {
            case LIST:
                pane_list.inc_cursor();
                break;
            case DETAIL:
                break;
            case BINARY:
                break;
            defaut:
                throw slankdev::exception("UNknown state");
                break;

        }
    }
    void press_k()
    {
        switch (cstate) {
            case LIST:
                pane_list.dec_cursor();
                break;
            case DETAIL:
                break;
            case BINARY:
                break;
            defaut:
                throw slankdev::exception("UNknown state");
                break;

        }
    }
    void press_enter()
    {
        switch (cstate) {
            case LIST:
            {
                if (pane_list.packets.empty()) return;
                Packet* pack = &pane_list.packets[pane_list.get_cursor()];
                pane_detail.print_packet_detail(pack);
                pane_binary.hex(pack);
                break;
            }
            case DETAIL:
                break;
            case BINARY:
                break;
            defaut:
                throw slankdev::exception("UNknown state");
                break;

        }
    }
    void view_help()
    {
        size_t x = 0;
        size_t y = 0;
        screen.mvprintw(y++, x, "                                                    ");
        screen.mvprintw(y++, x, "    CuiShark version 0.0                            ");
        screen.mvprintw(y++, x, "    Copyright 2017-2020 Hiroki SHIROKURA.           ");
        screen.mvprintw(y++, x, "                                                    ");
        screen.mvprintw(y++, x, "    Avalable Keyboard commands are below.           ");
        screen.mvprintw(y++, x, "    These are Vi-like key-bind because I'm vimmer.  ");
        screen.mvprintw(y++, x, "                                                    ");
        screen.mvprintw(y++, x, "    Commands                                        ");
        screen.mvprintw(y++, x, "    j      :  down                                  ");
        screen.mvprintw(y++, x, "    k      :  up                                    ");
        screen.mvprintw(y++, x, "    tab    :  switch pane                           ");
        screen.mvprintw(y++, x, "    enter  :  select                                ");
        screen.mvprintw(y++, x, "    ?      :  view help                             ");
        screen.mvprintw(y++, x, "    C-c    :  exit                                  ");
        screen.mvprintw(y++, x, "                                                    ");
        screen.getchar();
    }
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
                            press_j();
                            break;
                        case 'k':
                            press_k();
                            break;
                        case '\t':
                            charnge_cstate();
                            break;
                        case '\n':
                            press_enter();
                            break;
                        case '?':
                            view_help();
                            break;
                    }
                }
            }
            refresh();
        }
    }
    void refresh()
    {
        pane_list.refresh();
        pane_detail.refresh();
        pane_binary.refresh();
        statusline.refresh();
        screen.refresh();
    }
};


int main(int argc, char *argv[])
{
    display dsp0;
    dsp0.dispatch();
}



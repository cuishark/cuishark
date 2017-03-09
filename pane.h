
#pragma once

#include <pcap.h>
#include <slankdev/ncurses.h>
#include <slankdev/string.h>
#include <algorithm>
#include "packet.h"
#include "protocol.h"


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
        std::string str = slankdev::fs(fmt, arg...);
        screen.mvprintw(current_y, current_x, str.c_str());
        current_x += str.length();
        newline();
        attroff(A_REVERSE);
    }
    template <class... Arg>
    void print(const char* fmt, Arg... arg)
    {
        fflush(stdout);
        std::string str = slankdev::fs(fmt, arg...);
        screen.mvprintw(current_y, current_x, str.c_str());
        current_x += str.length();
    }
    void newline()
    {
        for (size_t i=current_x; i<w; i++) { print(" "); }
        current_x = x;
        current_y ++;
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
    std::vector<Packet*> packets;
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
    virtual ~Pane_list()
    {
        for (Packet* p : packets) {
            delete p;
        }
    }
    void push_packet(const void* packet, struct pcap_pkthdr* hdr)
    {
        static int number = 0;
        packets.push_back(new Packet(packet, hdr->len, hdr->ts.tv_sec, number++));
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
                println_hl(packets[i]->line().c_str());
            else
                println(packets[i]->line().c_str());
        }
    }
};







class Pane_detail : public pane {
    ssize_t cursor_index;
    size_t  list_start_index;
public:
    Packet* pack;

    ssize_t get_cursor() { return cursor_index; }
    void dec_cursor()
    {
        if (pack == nullptr) return;

        if (cursor_index-1 < list_start_index) scroll_up();
        if (get_cursor()-1 >= 0) cursor_index--;
    }
    void inc_cursor()
    {
        if (pack == nullptr) return;

        if (cursor_index+1-list_start_index >= h) scroll_down();
        if (get_cursor()+1 < pack->detail_lines.size()) cursor_index++;
    }
    void scroll_up()   { list_start_index --; }
    void scroll_down() { list_start_index ++; }

    Pane_detail(size_t ix, size_t iy, size_t iw, size_t ih,
            slankdev::ncurses& scr) : pane(ix, iy, iw, ih, scr), cursor_index(0),
            pack(nullptr) {}
    void add_analyze_result(Packet* pack);
    void refresh()
    {
        if (pack == nullptr)
            return;

        current_x = x;
        current_y = y;
        for (size_t i=0; i<pack->detail_lines.size(); i++) {
            if (i == get_cursor())
                println_hl(pack->detail_lines[i]->to_string().c_str());
            else
                println(pack->detail_lines[i]->to_string().c_str());

            if (pack->detail_lines[i]->get_state() == OPEN) {
                for (size_t c=0; c<pack->detail_lines[i]->childs.size(); c++) {
                    println("   %s", pack->detail_lines[i]->childs[c].c_str());
                }
            }
        }
        for (size_t i=0; i<h-pack->detail_lines.size(); i++) {
            for (size_t c=0; c<w; c++) print(" ");
            println("");
        }
    }
    void press_enter() { pack->detail_lines[get_cursor()]->openclose(); }
};


class Pane_binary : public pane {
    ssize_t cursor_index;
    size_t  list_start_index;
public:
    std::vector<std::string> lines;

    ssize_t get_cursor() { return cursor_index; }
    void dec_cursor()
    {
        if (cursor_index-1 < list_start_index) scroll_up();
        if (get_cursor()-1 >= 0) cursor_index--;
    }
    void inc_cursor()
    {
        if (cursor_index+1-list_start_index >= h) scroll_down();
        if (get_cursor()+1 < lines.size()) cursor_index++;
    }
    void scroll_up()   { list_start_index --; }
    void scroll_down() { list_start_index ++; }


    Pane_binary(size_t ix, size_t iy, size_t iw, size_t ih,
            slankdev::ncurses& scr) : pane(ix, iy, iw, ih, scr) {}
    void hex(Packet* packet)
    {
        lines.clear();
        using namespace slankdev;

        const void* buffer = packet->buf.data();
        ssize_t bufferlen   = packet->len;

        const uint8_t *data = reinterpret_cast<const uint8_t*>(buffer);
        size_t row = 0;
        std::string line;

        while (bufferlen > 0) {
            line.clear();

            line += fs(" %04zx   ", row);

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
                if (!(0x20 <= c && c <= 0x7e)) c = '.';
                line += fs("%c", c);
            }
            lines.push_back(line);
            bufferlen -= n;
            data += n;
            row  += n;
        }
    }
    void refresh()
    {
        current_x = x;
        current_y = y;
        size_t start_idx = list_start_index;
        for (size_t i=start_idx, c=0; i<lines.size() && c<h; i++, c++) {
            if (i == get_cursor())
                println_hl("%s", lines[i].c_str());
            else
                println("%s", lines[i].c_str());
        }
    }
};





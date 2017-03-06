
#pragma once


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




class Protocol {
    size_t analyze_icmp(const void* pkt, size_t len)
    {}
    size_t analyze_udp(const void* pkt, size_t len)
    {}
    size_t analyze_tcp(const void* pkt, size_t len)
    {}
    size_t analyze_arp(const void* pkt, size_t len)
    {}
    size_t analyze_ip(const void* pkt, size_t len)
    {}
    size_t analyze_ether(const void* pkt, size_t len)
    {
        using namespace slankdev;
        const ether* eth = reinterpret_cast<const ether*>(pkt);
        switch (ntohs(eth->type)) {
            case 0x0800:
            case 0x0806:
            case 0x86dd:
            default:
                break;
        }
    }
public:
    Protocol(const void* pkt, size_t len)
    {
        analyze_ether(pkt, len);
    }
    std::string line()
    {
    }
};


class Pane_detail : public pane {
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

    Pane_detail(size_t ix, size_t iy, size_t iw, size_t ih,
            slankdev::ncurses& scr) : pane(ix, iy, iw, ih, scr) {}
    void print_packet_detail(Packet* packet)
    {
        char str[1000];
        sprintf(str, "Packet Detail");
        lines.push_back(str);
        sprintf(str, " + number : %zd ", packet->number);
        lines.push_back(str);
        sprintf(str, " + pointer: %p", packet->buf);
        lines.push_back(str);
        sprintf(str, " + length : %zd", packet->len);
        lines.push_back(str);
        sprintf(str, " + time   : %ld", packet->time);
        lines.push_back(str);
    }
    void refresh()
    {
        current_x = x;
        current_y = y;
        for (size_t i=0, c=0; i<lines.size() && c<h; i++, c++) {
            if (i == get_cursor())
                println_hl("%s", lines[i].c_str());
            else
                println("%s", lines[i].c_str());
        }
        for (size_t i=0; i<h-lines.size(); i++) {
            for (size_t c=0; c<w; c++) print(" ");
            println("");
        }
    }
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

        const void* buffer = packet->buf;
        size_t bufferlen   = packet->len;
        char str[1000];

        const uint8_t *data = reinterpret_cast<const uint8_t*>(buffer);
        size_t row = 0;
        std::string line;
        while (bufferlen > 0) {
            line.clear();

            sprintf(str, " %04zx:   ", row);
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
                if (!(0x20 <= c && c <= 0x7e)) c = '.';
                sprintf(str, "%c", c);
                line += str;
            }
            println("");
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
        for (size_t i=0, c=0; i<lines.size() && c<h; i++, c++) {
            if (i == get_cursor())
                println_hl("%s", lines[i].c_str());
            else
                println("%s", lines[i].c_str());
        }
        for (size_t i=0; i<h-lines.size(); i++) {
            for (size_t c=0; c<w; c++) print(" ");
            println("");
        }
    }
};


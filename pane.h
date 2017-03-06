
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
        screen.mvprintw(current_y, current_x, fmt, arg...);
        current_x += len;
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



class line {
public:
    virtual ~line() {}
    virtual std::string to_string() = 0;
};


class staticline : public line {
    const std::string msg;
public:
    staticline(const char* str) : msg(str) {}
    std::string to_string() override { return msg; }
};




enum State {
    OPEN,
    CLOSE,
};

class Protoblock : public line {
    State state;
public:
    std::vector<line*> childs;
    State get_state() const { return state; }

    Protoblock() : state(CLOSE) {}
    void openclose()
    {
        switch (state) {
            case OPEN:
                state = CLOSE;
                break;
            case CLOSE:
                state = OPEN;
                break;
            default:
                throw slankdev::exception("UNKNOWN state");
        }
    }
};



class Ethernetblock : public Protoblock {
public:
    Ethernetblock(const void* ptr, size_t len, size_t* headerlen)
    {
        using namespace slankdev;
        childs.push_back(new staticline("dst"));
        childs.push_back(new staticline("src"));
        childs.push_back(new staticline("type"));
        *headerlen = sizeof(ether);
    }
    std::string to_string() { return "Ethernet"; }
};

class IP : public Protoblock {
public:
    IP(const void* ptr, size_t len, size_t* headerlen)
    {
        using namespace slankdev;
        childs.push_back(new staticline("src address"));
        childs.push_back(new staticline("dst address"));
        childs.push_back(new staticline("protocol"));
        *headerlen = (20);
    }
    std::string to_string() { return "Internet Protocol version 4"; }
};


class ARP : public Protoblock {
public:
    ARP(const void* ptr, size_t len, size_t* headerlen)
    {
        using namespace slankdev;
        childs.push_back(new staticline("Operation"));
        childs.push_back(new staticline("Source Hw address"));
        childs.push_back(new staticline("Target Hw address"));
        childs.push_back(new staticline("Source Proto address"));
        childs.push_back(new staticline("Target Proto address"));
        *headerlen = 0;
    }
    std::string to_string() { return "Adress Resolution Protocol"; }
};


class IPv6 : public Protoblock {
public:
    IPv6(const void* ptr, size_t len, size_t* headerlen)
    {
        using namespace slankdev;
        childs.push_back(new staticline("src address"));
        childs.push_back(new staticline("dst address"));
        childs.push_back(new staticline("protocol"));
        *headerlen = 0;
        throw slankdev::exception("IPV6 is not support yet.");
    }
    std::string to_string() { return "Internet Protocol version 6"; }
};


class TCP : public Protoblock {
public:
    TCP(const void* ptr, size_t len, size_t* headerlen)
    {
        using namespace slankdev;
        childs.push_back(new staticline("src port"));
        childs.push_back(new staticline("dst port"));
        childs.push_back(new staticline("sequence number"));
        childs.push_back(new staticline("acknoledge number"));
        childs.push_back(new staticline("window size"));
        childs.push_back(new staticline("flags"));
        childs.push_back(new staticline("checksum"));
        *headerlen = 0;
    }
    std::string to_string() { return "Transration Control Protocol"; }
};


class UDP : public Protoblock {
public:
    UDP(const void* ptr, size_t len, size_t* headerlen)
    {
        using namespace slankdev;
        childs.push_back(new staticline("src port"));
        childs.push_back(new staticline("dst port"));
        childs.push_back(new staticline("length"));
        childs.push_back(new staticline("checksum"));
        *headerlen = 0;
    }
    std::string to_string() { return "User Datagram Protocol"; }
};



class Binary : public Protoblock {
public:
    Binary()
    {
        using namespace slankdev;
        childs.push_back(new staticline("raw data"));
    }
    std::string to_string() { return "Binary Data"; }
};


class Pane_detail : public pane {
    ssize_t cursor_index;
    size_t  list_start_index;
public:
    std::vector<Protoblock*> lines;

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
            slankdev::ncurses& scr) : pane(ix, iy, iw, ih, scr), cursor_index(0) {}
    void add_analyze_result(Packet* pack)
    {
        using namespace slankdev;
        cursor_index = 0;
        lines.clear();

        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(pack->buf);
        size_t len = pack->len;

        size_t headerlen = 0;

        const ether* eth = reinterpret_cast<const ether*>(ptr);
        lines.push_back(new Ethernetblock(ptr, len, &headerlen));
        uint16_t type = ntohs(eth->type);
        len -= headerlen;
        ptr += headerlen;

        lines.push_back(new IP(ptr, len, &headerlen));
        lines.push_back(new ARP(ptr, len, &headerlen));
        lines.push_back(new Binary);

        if (type == 0x0800) {
            /*
             * Analyze IP
             */
            lines.push_back(new IP(ptr, len, &headerlen));
        } else if (type == 0x86dd) {
            /*
             * Analyze IPv6
             */
            lines.push_back(new IPv6(ptr, len, &headerlen));
        } else if (type == 0x0806) {
            /*
             * Analyze ARP
             */
            lines.push_back(new ARP(ptr, len, &headerlen));
        } else {
            lines.push_back(new Binary);
        }
    }
    void refresh()
    {
        current_x = x;
        current_y = y;
        for (size_t i=0; i<lines.size(); i++) {
            if (i == get_cursor())
                println_hl(lines[i]->to_string().c_str());
            else
                println(lines[i]->to_string().c_str());

            if (lines[i]->get_state() == OPEN) {
                for (size_t c=0; c<lines[i]->childs.size(); c++) {
                    println("   %s", lines[i]->childs[c]->to_string().c_str());
                }
            }
        }
        for (size_t i=0; i<h-lines.size(); i++) {
            for (size_t c=0; c<w; c++) print(" ");
            println("");
        }
    }
    void press_enter() { lines[get_cursor()]->openclose(); }
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


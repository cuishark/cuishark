
#pragma ocne

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
    slankdev::ncurses& screen;
public:
    Statusline(size_t ix, size_t iy, size_t iw, cursor_state& c,
            slankdev::ncurses& sc)
        : x(ix), y(iy), w(iw), state(c), screen(sc) {}
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
        move(y, x);
        attron(A_REVERSE);
        printline("[CUISHARK] pane=%s", state2str(state));
        attroff(A_REVERSE);
    }
    template <class... Arg>
    void printline(const char* fmt, Arg... arg)
    {
        move(y, x);
        for (size_t i=x; i<w; i++) { screen.printw(" "); }
        screen.mvprintw(y, x, fmt, arg...);
    }
};


size_t a(size_t h) { return (h-1) % 3; }
size_t m(size_t h) { return ((h-1)-((h-1)%3))/3; }
#define H screen.geth()

enum mode {
    fileif,
    netif,
};


class display {
    slankdev::ncurses screen;
    pcap_t *handle;
    cursor_state cstate;
public:
    Pane_list   pane_list;
    Pane_detail pane_detail;
    Pane_binary pane_binary;
    Statusline statusline;

    display(const char* ifname, mode md) :
        cstate(LIST),
        pane_list  (1, 1            , screen.getw()-2, a(H)+m(H)-1, screen),
        pane_detail(1, a(H)+m(H)+2  , screen.getw()-2, m(H)     -1, screen),
        pane_binary(1, a(H)+2*m(H)+3, screen.getw()-2, m(H)-2   -4, screen),
        statusline (1, a(H)+3*m(H)-1, screen.getw()-2, cstate     , screen)
    {
        char errbuf[PCAP_ERRBUF_SIZE];
        if (md == netif)
            handle = pcap_open_live(ifname, BUFSIZ, 0, 0, errbuf);
        else if (md == fileif)
            handle = pcap_open_offline(ifname, errbuf);
        else
            throw slankdev::exception("unknown mode");

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
            case LIST  : cstate = DETAIL; break;
            case DETAIL: cstate = BINARY; break;
            case BINARY: cstate = LIST  ; break;
            default : throw slankdev::exception("UNKNOWN state");
        }
    }
    void press_j()
    {
        switch (cstate) {
            case LIST  :
            {
                pane_list.inc_cursor();
                Packet* pack = pane_list.packets[pane_list.get_cursor()];
                pane_detail.add_analyze_result(pack);
                pane_binary.hex(pack);
                break;
            }
            case DETAIL: pane_detail.inc_cursor(); break;
            case BINARY: pane_binary.inc_cursor(); break;
            defaut: throw slankdev::exception("UNknown state");
        }
    }
    void press_k()
    {
        switch (cstate) {
            case LIST  :
            {
                pane_list.dec_cursor();
                Packet* pack = pane_list.packets[pane_list.get_cursor()];
                pane_detail.add_analyze_result(pack);
                pane_binary.hex(pack);
                break;
            }
            case DETAIL: pane_detail.dec_cursor(); break;
            case BINARY: pane_binary.dec_cursor(); break;
            defaut: throw slankdev::exception("UNknown state");
        }
    }
    void open_close()
    {
        switch (cstate) {
            case LIST:
            {
                if (pane_list.packets.empty()) return;
                Packet* pack = pane_list.packets[pane_list.get_cursor()];
                pane_detail.add_analyze_result(pack);
                pane_binary.hex(pack);
                break;
            }
            case DETAIL:
                pane_detail.press_enter();
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
            int res = poll(fds, 2, 1000);
            if (res < 0) {
                throw slankdev::exception("poll");
            } else {
                screen.clear();

                if (fds[0].revents & POLLIN) {
                    struct pcap_pkthdr header;
                    const u_char *packet;
                    packet = pcap_next(handle, &header);
                    if (packet == nullptr) {
                        pcap_close(handle);
                        fds[0].fd = -1;
                        static int c = 0;
                        c++;
                        if (c!=1) {
                            throw slankdev::exception("FFDFDFDF");
                        }

                    } else {
                        pane_list.push_packet(packet, &header);
                    }
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
                        case ' ':
                        case '\n':
                            open_close();
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
    void print_frame()
    {
        size_t x = 0;
        size_t y = 0;
        const size_t w = screen.getw();
        const size_t h = screen.geth();
        for (y = 0; y<h; y++) {
            for (x = 0; x<w; x++) {
                if (x==0 || x==w-1)
                    screen.mvprintw(y,x, "|");

                if ( (y==0) ||
                 (y==(pane_list.h + pane_list.y + 1)) ||
                 (y==(pane_detail.h + pane_detail.y + 1)) ||
                 (y==(pane_binary.h + pane_binary.y + 1)) ||
                 (y==h-1) ) {
                    screen.mvprintw(y,x, "-");
                }
            }
        }
    }
    void refresh()
    {
        static bool first = true;
        if (!pane_list.packets.empty() && first) {
            Packet* pack = pane_list.packets[pane_list.get_cursor()];
            pane_detail.add_analyze_result(pack);
            pane_binary.hex(pack);
            first = false;
        }
        pane_list.refresh();
        pane_detail.refresh();
        pane_binary.refresh();
        statusline.refresh();
        print_frame();
        screen.refresh();
    }
};



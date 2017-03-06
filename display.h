
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
        pane_list  (0, 0            , screen.getw(), a(H)+m(H), screen),
        pane_detail(0, a(H)+m(H)+1  , screen.getw(), m(H)     , screen),
        pane_binary(0, a(H)+2*m(H)+1, screen.getw(), m(H)     , screen),
        statusline (0, a(H)+3*m(H)  , screen.getw(), cstate)
    {
        char errbuf[PCAP_ERRBUF_SIZE];
        handle = pcap_open_live("wlx4ce676b67f5b", BUFSIZ, 0, 0, errbuf);
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
            case LIST  : cstate = DETAIL; break;
            case DETAIL: cstate = BINARY; break;
            case BINARY: cstate = LIST  ; break;
            default : throw slankdev::exception("UNKNOWN state");
        }
    }
    void press_j()
    {
        switch (cstate) {
            case LIST  : pane_list.inc_cursor()  ; break;
            case DETAIL: pane_detail.inc_cursor(); break;
            case BINARY: pane_binary.inc_cursor(); break;
            defaut: throw slankdev::exception("UNknown state");
        }
    }
    void press_k()
    {
        switch (cstate) {
            case LIST  : pane_list.dec_cursor()  ; break;
            case DETAIL: pane_detail.dec_cursor(); break;
            case BINARY: pane_binary.dec_cursor(); break;
            defaut: throw slankdev::exception("UNknown state");
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

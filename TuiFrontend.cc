
#include "TuiFrontend.h"

/*
 * Function Implementation
 */

void TuiFrontend::packet_input(const void* p, size_t l, uint64_t t)
{
  static size_t cnt = 1;
  pane1.packets.push_back(new Packet(p, l, t, cnt++));
}


void TuiFrontend::focuse_sw()
{
  switch (fstate) {
    case PANE1: fstate = PANE2; break;
    case PANE2: fstate = PANE3; break;
    case PANE3: fstate = PANE1; break;
    default: assert(false);
  }
}

FocusState TuiFrontend::fstate = PANE1;

#define sublines (LINES/3)
TuiFrontend::TuiFrontend()
  : pane1(0, sublines*0+2, COLS, sublines-2)
  , pane2(0, sublines*1+2, COLS, sublines-2)
  , pane3(0, sublines*2+2, COLS, sublines-2)
  , sline(0, sublines*3-1, COLS)
{
  attron(A_REVERSE);
  std::string s;

  s = slankdev::fs("%-5s %-13s %-20s %-20s %-6s %5s %-10s",
              "No.", "Time", "Source", "Destination", "Proto", "Len", "Info");
  while (s.size() < size_t(COLS-1)) s += ' ';
  mvprintw(sublines*0+1, 0, s.c_str());

  s = "Protocol Details";
  while (s.size() < size_t(COLS-1)) s += ' ';
  mvprintw(sublines*1+1, 0, s.c_str());

  s = "Binary Details";
  while (s.size() < size_t(COLS-1)) s += ' ';
  mvprintw(sublines*2+1, 0, s.c_str());

  attroff(A_REVERSE);

  pane1.init(stdscr);
  pane2.init(stdscr);
  pane3.init(stdscr);
  sline.init(stdscr);

  wrefresh(stdscr);
}
TuiFrontend::~TuiFrontend() {}
void TuiFrontend::init()
{
  initscr();
  noecho();
  keypad(stdscr, true);
  scrollok(stdscr, false);
}
void TuiFrontend::key_input(int c)
{
  if (c == '\t') {
    focuse_sw();
  } else {
    switch (fstate) {
      case PANE1:
        pane1.key_input(c);
        break;
      case PANE2:
        pane2.key_input(c);
        break;
      case PANE3:
        pane3.key_input(c);
        break;
      default:
      {
        printf("state %d \n", fstate);
        assert(false);
        break;
      }
    }
  }
}


void TuiFrontend::refresh()
{
  if (!pane1.packets.empty()) {
    pane1.refresh();
    size_t cur = pane1.cur();
    pane2.set_content(&pane1.packets.at(cur)->details);
    pane3.set_content(&pane1.packets.at(cur)->binarys);

    pane2.refresh();
    pane3.refresh();
  }

  sline.refresh();
}

void Statusline::refresh()
{
  std::string sss = slankdev::fs(
      "%s     Packets:%-5zd    IF[%s]     Filter[%s]",
      "CuiShark by @slankdev",
      info.nb_packet_recv,
      info.interface.c_str(),
      info.filterstring.c_str()
  );

#ifdef DEBUG
  static size_t cnt = 0;
  sss = std::to_string(cnt++) + " " + sss;
#endif

  while (sss.length() < size_t(COLS-1)) sss += " ";

  wattron(win, A_REVERSE);
  mvwprintw(win, 0, 0, "%s", sss.c_str());
  wattroff(win, A_REVERSE);

  wrefresh(win);
}


void PacketListPane::refresh()
{
  for (size_t i=start_idx, count=0; i<packets.size() && count<h; i++, count++) {
    if (i == cursor && TuiFrontend::fstate == PANE1) {
      wattron(win, A_BOLD);
      wattron(win, A_UNDERLINE);
    }

    std::string s = packets[i]->to_str();
    while (s.size() < this->w) s += ' ';
    mvwprintw(win, count, 0, "%s", s.c_str());

    if (i == cursor && TuiFrontend::fstate == PANE1) {
      wattroff(win, A_BOLD);
      wattroff(win, A_UNDERLINE);
    }
    clrtoeol();
  }
  wrefresh(win);
}


void ToggleListPane::refresh()
{
  assert(lines);
  werase(win);

  size_t count = 0;
  for (size_t i=start_idx; i<lines->size() && count<h; i++, count++) {
    wmove(win, count, 0);
    wclrtoeol(win);
    wrefresh(win);

    if (i == cursor && TuiFrontend::fstate == PANE2) {
      wattron(win, A_BOLD);
      wattron(win, A_UNDERLINE);
    }

    std::string s = lines->at(i)->to_string();
    while (s.size() < this->w) s += ' ';
    mvwprintw(win, count, 0, "%s", s.c_str());

    if (i == cursor && TuiFrontend::fstate == PANE2) {
      wattroff(win, A_BOLD);
      wattroff(win, A_UNDERLINE);
    }

    if (lines->at(i)->is_close() == false) {
      for (size_t j=0; j<lines->at(i)->lines.size(); j++) {
        count++;
        std::string& s = lines->at(i)->lines[j];
        mvwprintw(win, count, 0, "  %s", s.c_str());
      }
    }
  }

  wrefresh(win);
}


void TextPane::refresh()
{
  assert(lines);

  size_t count = 0;
  for (size_t i=start_idx ; i<lines->size() && count<h; i++, count++) {
    if (i == cursor && TuiFrontend::fstate == PANE3) {
      wattron(win, A_BOLD);
      wattron(win, A_UNDERLINE);
    }

    std::string s = lines->at(i);
    while (s.size() < this->w) s += ' ';
    mvwprintw(win, count, 0, "%s", s.c_str());

    if (i == cursor && TuiFrontend::fstate == PANE3) {
      wattroff(win, A_BOLD);
      wattroff(win, A_UNDERLINE);
    }

    clrtoeol();
  }

  /* fill space */
  std::string ls;
  while (ls.size() < this->w) ls += ' ';
  for (; count<h; count++) mvwprintw(win, count, 0, "%s", ls.c_str());

  wrefresh(win);
}



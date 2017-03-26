
#pragma once

#include <stdarg.h>
#include <slankdev/string.h>
#include <slankdev/ncurses.h>
#include "ncurses.h"
#include <string>
#include <vector>


class Statusline {
  WINDOW* win;
  const size_t x_pos;
  const size_t y_pos;
  const size_t w;
  std::string str;
 public:
  Statusline(WINDOW* rootwin, size_t ix_pos, size_t iy_pos, size_t iw)
    : x_pos(ix_pos),
    y_pos(iy_pos),
    w    (iw    )
  {
    win = slankdev::subwin(rootwin, 1, w, y_pos, x_pos);
  }
  template<class... ARGS>
  void print(const char* fmt, ARGS... args)
  {
    str = slankdev::fs(fmt, args...);
  }
  void refresh()
  {
    static size_t cnt = 0;
    mvwprintw(win, 0, 0, "%zd: %s", cnt, str.c_str());
    cnt ++;
    wrefresh(win);
  }

};



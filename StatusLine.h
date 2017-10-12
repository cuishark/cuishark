
#pragma once

#include <stdarg.h>
#include <slankdev/string.h>
#include <slankdev/ncurses.h>
#include "ncurses.h"
#include <string>
#include <vector>

class TuiFrontend;

class Statusline {
  WINDOW* win;
  size_t x;
  size_t y;
  size_t w;
 public:
  Statusline(size_t _x, size_t _y, size_t _w)
    : x(_x)
    , y(_y)
    , w(_w) {}
  void init(WINDOW* rw) { win = slankdev::subwin(rw, 1, w, y, x); }
  void resize(WINDOW* _rw, size_t _x, size_t _y, size_t _w)
  {
    if (!win) {
      printf("%s: bad window handle\n", __PRETTY_FUNCTION__);
      exit(1);
    }
    slankdev::delwin(win);
    win = nullptr;
    x = _x;
    y = _y;
    w = _w;
    win = slankdev::subwin(_rw, 1, w, y, x);
  }
  void refresh(); /* defined in TuiFrontend.h */
};



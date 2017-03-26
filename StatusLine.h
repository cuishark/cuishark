
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
  const size_t x;
  const size_t y;
  const size_t w;
  TuiFrontend* front;
 public:
  Statusline(size_t _x, size_t _y, size_t _w, TuiFrontend* _f)
    : x(_x)
    , y(_y)
    , w(_w)
    , front(_f) {}
  void init(WINDOW* rw) { win = slankdev::subwin(rw, 1, w, y, x); }
  void refresh(); /* defined in TuiFrontend.h */
};



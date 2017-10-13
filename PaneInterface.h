

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <ncurses.h>
#include "slankdev/ncurses.h"



/*
 * Abstraction Pane Class
 */
class PaneInterface {
 protected:
  WINDOW* win;
 public:
  size_t x;
  size_t y;
  size_t h;
  size_t w;
  PaneInterface(size_t _x, size_t _y, size_t _w, size_t _h)
    : win(nullptr)
    , x(_x)
    , y(_y)
    , h(_h)
    , w(_w) {}
  void init(WINDOW* _rw) { win = slankdev::subwin(_rw, h, w, y, x); }
  void resize(WINDOW* _rw, size_t _x, size_t _y, size_t _w, size_t _h)
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
    h = _h;
    win = slankdev::subwin(_rw, h, w, y, x);
  }
  virtual ~PaneInterface() {}
  virtual void refresh() = 0;
  virtual void key_input(int c) = 0;
};




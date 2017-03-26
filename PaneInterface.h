

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <ncurses.h>
#include "ncurses.h"



/*
 * Abstraction Pane Class
 */
class PaneInterface {
 protected:
  WINDOW* win;
 public:
  const size_t x;
  const size_t y;
  const size_t h;
  const size_t w;
  PaneInterface(size_t _x, size_t _y, size_t _w, size_t _h)
    : win(nullptr)
    , x(_x)
    , y(_y)
    , h(_h)
    , w(_w) {}
  void init(WINDOW* _rw) { win = slankdev::subwin(_rw, h, w, y, x); }
  virtual ~PaneInterface() {}
  virtual void refresh() = 0;
  virtual void key_input(char c) = 0;
};




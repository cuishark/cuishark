

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <ncurses.h>

#include "ncurses.h"



class Lines {
  std::vector<std::string> lines_;
 public:
  void addline(std::string s) { lines_.push_back(s); }
  std::string& operator[](size_t n) { return lines_.at(n); }
  size_t size() const { return lines_.size(); }
};


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
};


class Pane : public PaneInterface {
  Lines lines;
  size_t cursor;
  size_t start_idx;
 public:
  Pane(size_t ix_pos, size_t iy_pos, size_t iw, size_t ih)
    : PaneInterface(ix_pos, iy_pos, iw, ih), cursor(0), start_idx(0) {}
  template<class... ARGS>
  void print(const char* fmt, ARGS... args)
  {
    std::string s = slankdev::fs(fmt, args...);
    lines.addline(s);
  }
  virtual void refresh() override
  {
    for (size_t i=start_idx, count=0; i<lines.size() && count<h; i++, count++) {
      if (i == cursor) {
        wattron(win, A_REVERSE);
        std::string s = lines[i];
        while (s.size() < this->w) s += ' ';
        mvwprintw(win, count, 0, "%s", s.c_str());
        wattroff(win, A_REVERSE);
      } else {
        std::string s = lines[i];
        while (s.size() < this->w) s += ' ';
        mvwprintw(win, count, 0, "%s", s.c_str());
      }
      clrtoeol();
    }
    wrefresh(win);
  }
  void cursor_down()
  {
    if (cursor + 1 < lines.size()) {
      if (cursor - start_idx + 2 > h) scroll_down();
      cursor++;
    }
  }
  void cursor_up()
  {
    if (cursor > 0) {
      if (cursor - 1 < start_idx) scroll_up();
      cursor--;
    }
  }
 private:
  void scroll_down() { start_idx++; }
  void scroll_up()   { start_idx--; }

 /* Debug Functions */
 public:
  size_t cur() const { return cursor; }
  WINDOW* ww() { return win; }
};





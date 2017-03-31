
#pragma once

#include <stdint.h>
#include <stddef.h>

#include <string>

#include "ncurses.h"
#include "PaneInterface.h"


class TextPane : public PaneInterface {
  std::vector<std::string>* lines;
  size_t cursor;
  size_t start_idx;
 public:
  TextPane(size_t _x, size_t _y, size_t _w, size_t _h);
  virtual ~TextPane() {}

  virtual void refresh() override;
  virtual void key_input(int c) override;
  void cursor_down();
  void cursor_up();
  void set_content(std::vector<std::string>* l);

 private:
  void scroll_down() { start_idx++; }
  void scroll_up()   { start_idx--; }

 /* Debug Functions */
 public:
  size_t cur() const { return cursor; }
  WINDOW* ww() { return win; }
};




/*
 * Class Member Function Implementation
 */

void TextPane::set_content(std::vector<std::string>* l)
{
  if (l != lines) {
    cursor = 0;
    start_idx = 0;
    lines = l;
  }
}
void TextPane::key_input(int c)
{
  if (c == 'j' || c == KEY_DOWN) {
    cursor_down();
  } else if (c == 'k' || c == KEY_UP) {
    cursor_up();
  }
}
TextPane::TextPane(size_t _x, size_t _y, size_t _w, size_t _h)
  : PaneInterface(_x, _y, _w, _h), lines(nullptr), cursor(0), start_idx(0) {}
void TextPane::cursor_down()
{
  if (!lines) return ; // TODO: erase

  if (cursor + 1 < lines->size()) {
    if (cursor - start_idx + 2 > h) scroll_down();
    cursor++;
  }
}
void TextPane::cursor_up()
{
  if (cursor > 0) {
    if (cursor - 1 < start_idx) scroll_up();
    cursor--;
  }
}




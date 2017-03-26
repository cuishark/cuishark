
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
  virtual void key_input(char c) override;
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
void TextPane::key_input(char c)
{
  if (c == 'j') {
    cursor_down();
  } else if (c == 'k') {
    cursor_up();
  }
}
TextPane::TextPane(size_t _x, size_t _y, size_t _w, size_t _h)
  : PaneInterface(_x, _y, _w, _h), lines(nullptr), cursor(0), start_idx(0) {}
void TextPane::refresh()
{
  if (!lines) return ; // TODO: erase

  size_t count = 0;
  for (size_t i=start_idx ; i<lines->size() && count<h; i++, count++) {
    if (i == cursor) wattron(win, A_REVERSE);

    std::string s = lines->at(i);
    while (s.size() < this->w) s += ' ';
    mvwprintw(win, count, 0, "%s", s.c_str());

    if (i == cursor) wattroff(win, A_REVERSE);

    clrtoeol();
  }

  /* fill space */
  std::string ls;
  while (ls.size() < this->w) ls += ' ';
  for (; count<h; count++) mvwprintw(win, count, 0, "%s", ls.c_str());

  wrefresh(win);
}
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




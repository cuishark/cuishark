

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string>

#include "ncurses.h"
#include "PaneInterface.h"


class ToggleList_Element {
 public:
  std::vector<std::string> lines;
  virtual bool is_close() const = 0;
  virtual void toggle() = 0;
  virtual std::string to_string() const = 0;
};




class ToggleListPane : public PaneInterface {
  std::vector<ToggleList_Element*>* lines;
  size_t cursor;
  size_t start_idx;

  void scroll_down() { start_idx++; }
  void scroll_up()   { start_idx--; }
 public:
  ToggleListPane(size_t _x, size_t _y, size_t _w, size_t _h);
  virtual ~ToggleListPane() {}
  void set_content(std::vector<ToggleList_Element*>* l);

  size_t cur() { return cursor; }
  void cursor_down();
  void cursor_up();
  virtual void refresh() override;
  virtual void key_input(char c) override;
};



/*
 * Function Implementations
 */

void ToggleListPane::set_content(std::vector<ToggleList_Element*>* l)
{
  if (l != lines) {
    cursor = 0;
    start_idx = 0;
    lines = l;
  }
}
void ToggleListPane::key_input(char c)
{
  if (c == 'j') {
    cursor_down();
  } else if (c == 'k') {
    cursor_up();
  } else if (c == ' ') {
    lines->at(cursor)->toggle();
  }
}


ToggleListPane::ToggleListPane(size_t _x, size_t _y, size_t _w, size_t _h)
  : PaneInterface(_x, _y, _w, _h), lines(nullptr), cursor(0), start_idx(0) {}


void ToggleListPane::refresh()
{
  if (lines == nullptr) return ;

  size_t count = 0;
  for (size_t i=start_idx; i<lines->size() && count<h; i++, count++) {
    if (i == cursor) wattron(win, A_REVERSE);

    std::string s = lines->at(i)->to_string();
    while (s.size() < this->w) s += ' ';
    mvwprintw(win, count, 0, "%s", s.c_str());
    clrtoeol();

    if (i == cursor) wattroff(win, A_REVERSE);

    if (lines->at(i)->is_close() == false) {
      for (size_t j=0; j<lines->at(i)->lines.size(); j++) {
        count++;
        std::string s = lines->at(i)->lines[j];
        while (s.size() < this->w) s += ' ';
        mvwprintw(win, count, 0, "  %s", s.c_str());
      }
    }
  }

  /* fill space */
  std::string ls;
  while (ls.size() < this->w) ls += ' ';
  for (; count<h; count++) mvwprintw(win, count, 0, "%s", ls.c_str());

  wrefresh(win);
}
void ToggleListPane::cursor_down()
{
  if (cursor + 1 < lines->size()) {
    if (cursor - start_idx + 2 > h) scroll_down();
    cursor++;
  }
}
void ToggleListPane::cursor_up()
{
  if (cursor > 0) {
    if (cursor - 1 < start_idx) scroll_up();
    cursor--;
  }
}




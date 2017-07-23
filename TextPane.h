
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





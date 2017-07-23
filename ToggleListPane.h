

#pragma once

#include <vector>
#include <string>
#include <stdint.h>
#include <stddef.h>
#include "PaneInterface.h"


class ToggleList_Element {
 public:
  std::vector<std::string> lines;
  virtual ~ToggleList_Element() {}
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
  virtual void key_input(int c) override;
};




#include "ToggleListPane.h"
#include "ncurses.h"


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
void ToggleListPane::key_input(int c)
{
  if (c == 'j' || c == KEY_DOWN) {
    cursor_down();
  } else if (c == 'k' || c == KEY_UP) {
    cursor_up();
  } else if (c == ' ') {
    lines->at(cursor)->toggle();
  }
}


ToggleListPane::ToggleListPane(size_t _x, size_t _y, size_t _w, size_t _h)
  : PaneInterface(_x, _y, _w, _h), lines(nullptr), cursor(0), start_idx(0) {}


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




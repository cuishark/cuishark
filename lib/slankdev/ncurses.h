
#pragma once

#include <ncurses.h>
#include <slankdev/string.h>
#include <slankdev/exception.h>


namespace slankdev {

inline WINDOW* newwin(size_t nlines, size_t ncols, size_t begin_y, size_t begin_x)
{
  WINDOW* win = ::newwin(nlines, ncols, begin_y, begin_x);
  if (win == NULL) {
    std::string errstr = slankdev::fs(
                      "newwin(nlines=%zd,ncols=%zd,y=%zd,x=%zd)",
                                nlines, ncols, begin_y, begin_x);
    throw slankdev::exception(errstr.c_str());
  }
  return win;
}

inline WINDOW* subwin(WINDOW* win, size_t nlines, size_t ncols, size_t begin_y, size_t begin_x)
{
  WINDOW* sub = ::subwin(win, nlines, ncols, begin_y, begin_x);
  if (sub == NULL) {
    std::string errstr = slankdev::fs(
                      "subwin(nlines=%zd,ncols=%zd,y=%zd,x=%zd)",
                                nlines, ncols, begin_y, begin_x);
    throw slankdev::exception(errstr.c_str());
  }
  return sub;
}



} // namespace




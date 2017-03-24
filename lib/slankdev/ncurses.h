


#pragma once
#include <ncurses.h>

namespace slankdev {

class ncurses {
private:
    size_t scrn_width;
    size_t scrn_height;
public:
    ncurses()
    {
        initscr();
        getmaxyx(stdscr, scrn_height, scrn_width);
        erase();
    }
    ~ncurses()
    {
        endwin();
    }
    size_t getw() const { return scrn_width ; }
    size_t geth() const { return scrn_height; }

    template<class... Args>
    static void mvprintw(size_t y, size_t x, const char* fmt, Args... args)
    {
        ::mvprintw(y, x, fmt, args...);
    }
    template<class... Args>
    static void printw(const char* fmt, Args... args)
    {
        ::printw(fmt, args...);
    }
    static void refresh() { ::refresh(); }
    static char getchar() { return ::getch(); }
    static void clear() { ::clear(); }
};


} /* namespace slankdev */

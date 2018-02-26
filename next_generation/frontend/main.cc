

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>


int main(int argc, char** argv)
{
  int  x, y, w, h;
	const char *str = "Hello World";

	initscr();
	getmaxyx(stdscr, h, w);
	y = h/2;
	x = (w - strlen(str))/2;

	move(y, x);
	addstr(str);
	getch();

	endwin();
}


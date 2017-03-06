

all:
	g++ -std=c++11 -Wno-format-security main.cc -o cuishark -lncurses -lpcap

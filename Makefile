

all:
	g++ -std=c++11 \
		-Wno-format-security -Iinclude \
		main.cc -o cuishark \
		-lncurses -lpcap

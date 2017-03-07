

all:
	g++ -std=c++11 \
		-Wno-format-security -Iinclude \
		main.cc -o cuishark \
		-lncurses -lpcap


gdb:
	sudo gdb --args \
		./cuishark wlp3s0

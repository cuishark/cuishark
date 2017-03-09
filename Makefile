

GDB =
# GDB = gdb --args
WLN = wlp2s0


CXX = g++
SRC = main.cc
OBJ = $(SRC:.cc=.c)
TARGET = cuishark

CXXFLAGS +=  -std=c++11 -Wno-format-security -g -O0
LDFLAGS  = -lncurses -lpcap


all:
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS)

pcap:
	sudo $(GDB) ./$(TARGET) pcap in.pcap

lo:
	sudo $(GDB) ./$(TARGET) net lo

wlan:
	sudo $(GDB) ./$(TARGET) net $(WLN)



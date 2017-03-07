
MODE    := pcap
INPUTIF := in.pcap

CXX = g++
SRC = main.cc
OBJ = $(SRC:.cc=.c)
TARGET = cuishark

CXXFLAGS +=  -std=c++11 -Wno-format-security -g -O0
CXXFLAGS += -Iinclude

LDFLAGS  = -lncurses -lpcap


all:
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS)

net:
	sudo ./$(TARGET) net wlp3s0


gdb:
	gdb --args ./$(TARGET) $(MODE) $(INPUTIF)

run:
	sudo ./$(TARGET) $(MODE) $(INPUTIF)




CXX = g++
SRC = main.cc
TARGET = cuishark

CXXFLAGS += -std=c++11 -Wno-format-security -g -O0 -I./lib
LDFLAGS  += -lncurses -lpcap


all:
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -rf $(TARGET)

install:
	cp $(TARGET) /usr/local/bin/$(TARGET)

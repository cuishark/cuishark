

SRC = main.cc
TARGET = cuishark

CXXFLAGS += -std=c++11 -Wno-format-security -g -O0 -I./lib -Wall -Werror
LDFLAGS  += -lncurses -lpcap

all:
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -rf $(TARGET)

install:
	cp $(TARGET) /usr/local/bin/$(TARGET)



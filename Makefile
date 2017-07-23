

SRC = main.cc
TARGET = cuishark

CXX = clang++
CXXFLAGS += -Wall -Werror
# ifeq ($(CXX), clang++)
# CXXFLAGS += -Weverything
# endif
CXXFLAGS += -std=c++11 -g -O0 -I./lib
LDFLAGS  += -lncurses -lpcap

all:
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -rf $(TARGET)

install:
	cp $(TARGET) /usr/local/bin/$(TARGET)



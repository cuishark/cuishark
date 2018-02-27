
LIBSLANKDEV := $(HOME)/git/libslankdev
CXXFLAGS += -std=c++11 -g -O0
CXXFLAGS += -I$(LIBSLANKDEV)
CXXFLAGS += -I/usr/local/include/wireshark
CXXFLAGS += `pkg-config glib-2.0 --cflags`
LDFLAGS  += `pkg-config glib-2.0 --libs`
LDFLAGS  += -lwiretap -lwireshark -lwsutil

SRC = main.cc
OBJ = $(SRC:.cc=.o)
TARGET = a.out

$(TARGET): $(OBJ)

clean:
	rm -f *.o *.out

run:
	./a.out 2>/dev/null

include $(LIBSLANKDEV)/mk/rules.mk


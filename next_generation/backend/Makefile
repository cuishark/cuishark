
LIBSLANKDEV := $(HOME)/git/libslankdev
CXXFLAGS += -I$(LIBSLANKDEV) -std=c++11 -g -O0
CXXFLAGS += `pkg-config wireshark --cflags`
LDFLAGS  += `pkg-config wireshark --libs`
LDFLAGS  += -lwiretap -lwireshark -lwsutil

SRC = main.cc
OBJ = $(SRC:.cc=.o)
TARGET = a.out

$(TARGET): $(OBJ)

clean:
	rm -f *.o *.out

include $(LIBSLANKDEV)/mk/rules.mk


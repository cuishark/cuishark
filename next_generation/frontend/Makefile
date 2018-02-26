
LIBSLANKDEV := $(HOME)/git/libslankdev
CXXFLAGS += -I$(LIBSLANKDEV) -std=c++11
LDFLAGS  += -lncurses

SRC = main.cc
OBJ = $(SRC:.cc=.o)
TARGET = a.out

$(TARGET): $(OBJ)

clean:
	rm -f *.o *.out

include $(LIBSLANKDEV)/mk/rules.mk


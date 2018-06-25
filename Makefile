PREFIX = /usr/local

CXX = g++
CXXFLAGS += -Wall -Werror
ifeq ($(CXX), clang++)
# CXXFLAGS += -Weverything
# CXXFLAGS += -Wno-format-security       \
# 						-Wno-c++98-compat          \
# 						-Wno-c++98-compat-pedantic \
# 						-Wno-format-nonliteral
endif
CXXFLAGS += -std=c++11 -g -O0 -I./lib
LDFLAGS  += -lncurses -lpcap

SRC = main.cc \
			TuiFrontend.cc \
			TextPane.cc \
			PacketListPane.cc \
			ToggleListPane.cc \
			protocol.cc
OBJ = $(SRC:.cc=.o)
TARGET = cuishark

.cc.o:
	@echo "CXX $@"
	@$(CXX) $(CXXFLAGS) $< -c -o $@

$(TARGET): $(OBJ)
	@echo "LD $@"
	@$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -rf $(TARGET) *.o

install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)



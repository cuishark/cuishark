
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string>
#include <vector>
#include "PaneInterface.h"

class ToggleList_Element;

class Packet {
  std::vector<uint8_t> buf;
  size_t  len;
  uint64_t time;
  size_t number;
  std::string src;
  std::string dst;
  std::string protocol;
  std::string msg;
  std::string summary;

 public:
  std::vector<ToggleList_Element*>  details;
  std::vector<std::string> binarys;

  Packet(const void* p, size_t l, uint64_t t, size_t n);
  virtual ~Packet();
  std::string to_str() const;

 private:
  void analyze(const uint8_t* ptr, size_t len);
};


class PacketListPane : public PaneInterface {
  size_t cursor;
  size_t start_idx;
 public:
  std::vector<Packet*> packets;
  PacketListPane(size_t _x, size_t _y, size_t _w, size_t _h);

 public:
  virtual void refresh() override;
  virtual void key_input(int c) override;
  void cursor_down();
  void cursor_up();

 private:
  void scroll_down() { start_idx++; }
  void scroll_up()   { start_idx--; }

 /* Debug Functions */
 public:
  size_t cur() const { return cursor; }
  WINDOW* ww() { return win; }
};




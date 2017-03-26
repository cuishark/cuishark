
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string>
#include <vector>

#include "TextPane.h"
#include "protocol.h"

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
  virtual ~Packet()
  {
    for (size_t i=0; i<details.size(); i++) {
      delete details[i];
    }
  }

  std::string to_str() const;

 private:
  void analyze(const uint8_t* ptr, size_t len);
};

void Packet::analyze(const uint8_t* ptr, size_t len)
{
  using namespace slankdev;
  details.clear();

  Ethernet* eth = new Ethernet(ptr, len);
  details.push_back(eth);
  len -= eth->headerlen();
  ptr += eth->headerlen();
  uint16_t type = eth->type();
  src = eth->src();
  dst = eth->dst();
  protocol = "Ether";
  summary  = fs("Ethernet packet.type=0x%04x", eth->type());

  switch (type) {
    case 0x800:
      {
        IP* ip = new IP(ptr, len);
        details.push_back(ip);
        len -= ip->headerlen();
        ptr += ip->headerlen();
        uint8_t proto = ip->protocol();
        src = ip->src();
        dst = ip->dst();
        protocol = "IPv4";
        summary = fs("protocol=%u\n", ip->protocol());

        switch (proto) {
          case 1:
            {
              ICMP* icmp = new ICMP(ptr, len);
              details.push_back(icmp);
              len -= icmp->headerlen();
              ptr += icmp->headerlen();
              protocol = "ICMP";
              summary = fs("type=%u code=%u", icmp->type(), icmp->code());
              break;
            }
          case 17:
            {
              UDP* udp = new UDP(ptr, len);
              details.push_back(udp);
              len -= udp->headerlen();
              ptr += udp->headerlen();
              protocol = "UDP";
              summary = fs("%u -> %u", udp->src(), udp->dst());
              break;
            }
          case 6:
            {
              TCP* tcp = new TCP(ptr, len);
              details.push_back(tcp);
              len -= tcp->headerlen();
              ptr += tcp->headerlen();
              protocol = "TCP";
              summary = fs("%u -> %u", tcp->src(), tcp->dst());
              break;
            }
        }
        break;
      }
    case 0x86dd:
      {
        /*
         * Analyze IPv6
         */
        protocol = "IPv6";
        break;
      }
    case 0x0806:
      {
        /*
         * Analyze ARP
         */
        ARP* arp = new ARP(ptr, len);
        details.push_back(arp);
        len -= arp->headerlen();
        ptr += arp->headerlen();
        protocol = "ARP";
        break;
      }
  }
  if (len > 0) details.push_back(new Binary(ptr, len));
}


Packet::Packet(const void* p, size_t l, uint64_t t, size_t n)
  : len(l), time(t),  number(n)
{
  /*
   * Init Detail Pane Information
   */
  analyze(reinterpret_cast<const uint8_t*>(p), l);

  /*
   * Init Binary Pane Information
   */
  binarys.clear();
  using namespace slankdev;

  const void* buffer = p;
  ssize_t bufferlen  = l;

  const uint8_t *data = reinterpret_cast<const uint8_t*>(buffer);
  size_t row = 0;

  std::string line;
  while (bufferlen > 0) {
    line.clear();

    line += fs(" %04zx   ", row);

    size_t n = std::min(bufferlen, ssize_t(16));

    for (size_t i = 0; i < n; i++) {
      if (i == 8) { line += " "; }
      line += fs(" %02x", data[i]);
    }
    for (size_t i = n; i < 16; i++) { line += "   "; }
    line += "   ";
    for (size_t i = 0; i < n; i++) {
      if (i == 8) { line += "  "; }
      uint8_t c = data[i];
      if (!(0x20 <= c && c <= 0x7e)) c = '.';
      line += fs("%c", c);
    }
    binarys.push_back(line);
    bufferlen -= n;
    data += n;
    row  += n;
  }
}


class PacketListPane : public PaneInterface {
  size_t cursor;
  size_t start_idx;
 public:
  std::vector<Packet*> packets;
  PacketListPane(size_t _x, size_t _y, size_t _w, size_t _h);

 public:
  template<class... ARGS> void print(const char* fmt, ARGS... args);
  virtual void refresh() override;
  virtual void key_input(char c) override;
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





/*
 * Class Member Function Implementation
 */
std::string Packet::to_str() const
{
  using namespace slankdev;
  char sstr[1000];
  sprintf(sstr, "%5zd %-13ld %-20s %-20s %6s %5zd %-10s" , number, time,
          src.c_str(),
          dst.c_str(),
          protocol.c_str(), len,
          summary.c_str());
  return sstr;
}


void PacketListPane::key_input(char c)
{
  if (c == 'j') {
    cursor_down();
  } else if (c == 'k') {
    cursor_up();
  }
}

PacketListPane::PacketListPane(size_t _x, size_t _y, size_t _w, size_t _h)
  : PaneInterface(_x, _y, _w, _h), cursor(0), start_idx(0) {}


void PacketListPane::refresh()
{
  for (size_t i=start_idx, count=0; i<packets.size() && count<h; i++, count++) {
    if (i == cursor) wattron(win, A_REVERSE);

    std::string s = packets[i]->to_str();
    while (s.size() < this->w) s += ' ';
    mvwprintw(win, count, 0, "%s", s.c_str());

    if (i == cursor) wattroff(win, A_REVERSE);
    clrtoeol();
  }
  wrefresh(win);
}
void PacketListPane::cursor_down()
{
  if (cursor + 1 < packets.size()) {
    if (cursor - start_idx + 2 > h) scroll_down();
    cursor++;
  }
}
void PacketListPane::cursor_up()
{
  if (cursor > 0) {
    if (cursor - 1 < start_idx) scroll_up();
    cursor--;
  }
}



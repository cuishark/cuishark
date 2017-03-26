


#include <string>
#include <slankdev/string.h>
#include <slankdev/poll.h>
#include <slankdev/pcap.h>
#include "TuiFrontend.h"



size_t packet_recv_count = 0;


class Pcap : public slankdev::pcap {
  TuiFrontend* front;
 public:
  Pcap(TuiFrontend* f) : front(f) {}
  virtual void recv(const void* ptr, struct pcap_pkthdr* hdr) override
  { front->packet_input(ptr, hdr->len, hdr->ts.tv_sec); }
};


int	main(int argc, char** argv)
{
  TuiFrontend::init();
  TuiFrontend fe;

  Pcap pcapfd(&fe);
  pcapfd.open_live("lo");

  fe.refresh();
  struct pollfd fds[2];
  memset(fds, 0, sizeof(fds));
  fds[0].fd = fileno(stdin);
  fds[0].events = POLLIN;
  fds[1].fd = pcapfd.get_selectable_fd();
  fds[1].events = POLLIN;

	while (1) {
    slankdev::poll(fds, sizeof(fds)/sizeof(fds[0]), 100);
    if (fds[0].revents & POLLIN) {
      fe.key_input(wgetch(stdscr));
      fe.refresh();
    } else if (fds[1].revents & POLLIN) {
      packet_recv_count ++;
      pcapfd.next();
      fe.sline.refresh();
    }
	}
	endwin();
}



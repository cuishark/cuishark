


#include <string>
#include <unistd.h>
#include <slankdev/string.h>
#include <slankdev/poll.h>
#include <slankdev/pcap.h>
#include "TuiFrontend.h"



size_t packet_recv_count = 0;


class Pcap : public slankdev::pcap {
  TuiFrontend* front;
 public:
  Pcap() : front(nullptr) {}
  virtual void recv(const void* ptr, struct pcap_pkthdr* hdr) override
  { front->packet_input(ptr, hdr->len, hdr->ts.tv_sec); }
  void register_frontend(TuiFrontend* f) { front = f; }
  virtual void open_live(const char* ifname) override
  {
    try {
      slankdev::pcap::open_live(ifname);
    } catch (std::exception& e) {
      printf("cuishark: %s\n", e.what());
      exit(-1);
    }
  }
  virtual void open_offline(const char* ifname) override
  {
    try {
      slankdev::pcap::open_offline(ifname);
    } catch (std::exception& e) {
      printf("cuishark: %s\n", e.what());
      exit(-1);
    }
  }
};



enum inputmode {
  NONE,
  PCAP,
  NETIF,
};

class OptParser {
  inputmode mode_;
  std::string inputname_;
 public:
  OptParser(int argc, char** argv) : mode_(NONE)
  {
    int res;
    while ((res = getopt(argc, argv, "r:i:")) != -1) {
      switch (res) {
        case 'r':
          this->mode_      = PCAP;
          this->inputname_ = optarg;
          break;
        case 'i':
          this->mode_      = NETIF;
          this->inputname_ = optarg;
          break;
        default:
          exit(-1);
      }
    }
  }
  inputmode mode() const { return mode_; }
  const std::string& inputname() const { return inputname_; }
};


void usage(const char* progname)
{
  printf("Usage: %s [-r PCAPFILE] [-i NETIF] \n", progname);
}


int	main(int argc, char** argv)
{
  Pcap pcapfd;
  OptParser opt(argc, argv);
  switch (opt.mode()) {
    case PCAP:
      pcapfd.open_offline(opt.inputname().c_str());
      break;
    case NETIF:
      pcapfd.open_live(opt.inputname().c_str());
      break;
    default:
      fprintf(stderr, "input interface is not specilied.\n");
      usage(argv[0]);
      return -1;
      break;
  }

  TuiFrontend::init();
  TuiFrontend fe;
  pcapfd.register_frontend(&fe);

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
      try {
        pcapfd.next();
      } catch (std::exception& e) {
        pcapfd.close();
        fds[1].fd = -1;
      }
      fe.refresh();
    }
	}
	endwin();
}



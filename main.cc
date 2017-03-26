


#include <string>
#include <unistd.h>
#include <slankdev/string.h>
#include <slankdev/poll.h>
#include <slankdev/pcap.h>
#include "TuiFrontend.h"
#include "Cuishark.h"



CuisharkInfo info;

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
  std::string filter_;
  bool autoscroll_;
  bool filter_on_;
 public:
  OptParser(int argc, char** argv)
    : mode_(NONE), autoscroll_(false), filter_on_(false)
  {
    int res;
    while ((res = getopt(argc, argv, "r:i:f:a")) != -1) {
      switch (res) {
        case 'r':
          this->mode_      = PCAP;
          this->inputname_ = optarg;
          break;
        case 'i':
          this->mode_      = NETIF;
          this->inputname_ = optarg;
          break;
        case 'f':
          filter_on_  = true;
          filter_ = optarg;
          break;
        case 'a':
          autoscroll_ = true;
          break;
        default:
          exit(-1);
      }
    }
  }
  inputmode mode() const { return mode_; }
  const std::string& inputname() const { return inputname_; }
  const std::string& filter()   const { return filter_; }
  bool autoscroll() const { return autoscroll_; }
  bool filter_on() const { return filter_on_; }
};


void usage(const char* progname)
{
  printf("Usage: %s [a] [-r PCAPFILE] [-i NETIF] [-f FilterSyntax] \n", progname);
  printf("   -r PCAPFILE -- input interface is pcapfile\n");
  printf("   -i NETIF    -- input interface is network interface\n");
  printf("   -f SYNTAX   -- compile and use PCAP-CAPTHRE-FILTER \n");
  printf("   -a          -- automaticaly to scroll\n");
}


int	main(int argc, char** argv)
{
  Pcap pcapfd;
  OptParser opt(argc, argv);
  switch (opt.mode()) {
    case PCAP:
      pcapfd.open_offline(opt.inputname().c_str());
      info.interface = opt.inputname();
      break;
    case NETIF:
      pcapfd.open_live(opt.inputname().c_str());
      info.interface = opt.inputname();
      break;
    default:
      fprintf(stderr, "input interface is not specilied.\n");
      usage(argv[0]);
      return -1;
      break;
  }

  if (opt.filter_on()) {
    struct bpf_program fp;
    try {
      pcapfd.compile(&fp, opt.filter().c_str(), 0, 0);
      pcapfd.setfilter(&fp);
    } catch (std::exception& e) {
      fprintf(stderr, e.what());
      return -1;
    }
    info.filterstring = opt.filter();
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
      try {

        info.nb_packet_recv ++;
        pcapfd.next();
        fe.refresh();
        if (opt.autoscroll()) fe.pane1.cursor_down();

      } catch (std::exception& e) {
        pcapfd.close();
        fds[1].fd = -1;
      }
    }
	}
	endwin();
}



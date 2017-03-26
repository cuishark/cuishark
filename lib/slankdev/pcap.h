
#pragma once

#include <pcap.h>
#include <slankdev/exception.h>
#include <slankdev/util.h>
#include <slankdev/string.h>

namespace slankdev {


class pcap {
  pcap_t* handle;
 public:
  pcap() : handle(nullptr) {}
  virtual ~pcap() { close(); }

  int get_selectable_fd()
  {
    int res = pcap_get_selectable_fd(handle);
    if (res < 0) {
      throw slankdev::exception("pcap_get_selectable_fd");
    }
    return res;
  }
  virtual void open_live(const char* ifname)
  {
    if (handle)
      throw slankdev::exception("pcap: exist open discriptor");

    char errbuf[PCAP_ERRBUF_SIZE];
    handle = pcap_open_live(ifname, BUFSIZ, 0, 0, errbuf);
    if (!handle) {
      std::string errstr;
      errstr = slankdev::fs("pcap_open_live: %s", errbuf);
      throw slankdev::exception(errstr.c_str());
    }
  }
  virtual void open_offline(const char* filepath)
  {
    if (handle)
      throw slankdev::exception("pcap: exist open discriptor");

    char errbuf[PCAP_ERRBUF_SIZE];
    handle = pcap_open_offline(filepath, errbuf);
    if (!handle) {
      std::string errstr;
      errstr = slankdev::fs("pcap_open_offline: %s", errbuf);
      throw slankdev::exception(errstr.c_str());
    }
  }
  virtual void close()
  {
    if (handle) {
      pcap_close(handle);
      handle = nullptr;
    }
  }
  void next()
  {
    struct pcap_pkthdr header;
    const u_char *packet = pcap_next(handle, &header);
    if (packet == nullptr) {
      throw slankdev::exception("pcap_next: recvmiss?");
    }
    recv(packet, &header);
  }
  virtual void recv(const void* ptr, struct pcap_pkthdr* header)
  {
    slankdev::hexdump("Userfunction", ptr, header->len);
  }
  void setfilter(struct bpf_program *fp)
  {
    int ret = pcap_setfilter(handle, fp);
    if (ret < 0) {
      std::string errstr = pcap_geterr(handle);
      throw slankdev::exception(errstr.c_str());
    }
  }
  void compile(struct bpf_program *fp,
      const char *str, int optimize, bpf_u_int32 netmask)
  {
    int ret = pcap_compile(handle, fp, str, optimize, netmask);
    if (ret < 0) {
      std::string errstr = pcap_geterr(handle);
      throw slankdev::exception(errstr.c_str());
    }
  }

};


} /* namespace slankadev */

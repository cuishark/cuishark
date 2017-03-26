
#pragma once

#include <stdint.h>
#include <stddef.h>




class CuisharkInfo {
 public:
  std::string interface;
  std::string filterstring;
  size_t nb_packet_recv;
  CuisharkInfo() : nb_packet_recv(0) {}
};


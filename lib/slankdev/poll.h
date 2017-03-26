

#pragma once

#include <poll.h>
#include <slankdev/exception.h>

namespace slankdev {

void poll(struct ::pollfd* fds, size_t nfds, int timeout)
{
  int res = ::poll(fds, nfds, timeout);
  if (res < 0) {
    throw slankdev::exception("poll");
  }
}

} /* namespace slankdev */

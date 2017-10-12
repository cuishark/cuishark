

#pragma once

#include <iostream>
#include <poll.h>
#include <slankdev/exception.h>
#include <errno.h>

namespace slankdev {

static inline void poll(struct ::pollfd* fds, size_t nfds, int timeout)
{
  int res = ::poll(fds, nfds, timeout);
  if (res < 0) {
	// When window is resized, process receives a WINCH signal that
	// interrupts it. Let's mark this case as non-fatal for now.
	if (EINTR != errno) {
		throw slankdev::exception("poll");
	}
  }
}

} /* namespace slankdev */

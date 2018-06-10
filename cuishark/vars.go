
package main

import "github.com/cuishark/cuishark/cgocuishark"

var (
  viewArr = []string{"PacketList", "PacketDetail", "PacketByte"}
  active  = 0
  packets = []cgocuishark.Packet{}
  auto_scroll = false
  ifname = "????"
  first = true
)





#include <pcap.h>
#include <stdio.h>
#include <poll.h>

#include <vector>
#include <queue>
#include <algorithm>

#include <slankdev/util.h>
#include <slankdev/ncurses.h>
#include <slankdev/exception.h>
#include <slankdev/net/protocol.h>

#include "packet.h"
#include "pane.h"
#include "display.h"



int main(int argc, char *argv[])
{
    display dsp0;
    dsp0.dispatch();
}



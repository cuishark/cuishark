


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
    if (argc < 3) {
        fprintf(stderr, "Usage: %s [net|pcap] interface\n", argv[0]);
        return -1;
    }

    mode m;
    std::string s = argv[1];
    if (s == "net")       m = netif;
    else if (s == "pcap") m = fileif;
    else {
        fprintf(stderr, "mode not support\n");
        return -1;
    }

    display dsp0(argv[2], m);
    dsp0.dispatch();
}



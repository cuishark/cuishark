
#pragma once

#include <stddef.h>
#include <stdio.h>



inline void version()
{
  printf("CuiShark 0.1\n");
  printf("A protocol analyzer like a Wireshark on CUI/TUI.\n");
  printf("\n");
  printf("MIT License\n");
  printf("Copyright (c) 2017 Hiroki SHIROKURA\n");
  printf("\n");
  printf("Permission is hereby granted, free of charge, to any person obtaining a copy\n");
  printf("of this software and associated documentation files (the \"Software\"), to deal\n");
  printf("in the Software without restriction, including without limitation the rights\n");
  printf("to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n");
  printf("copies of the Software, and to permit persons to whom the Software is\n");
  printf("furnished to do so, subject to the following conditions:\n");
  printf("\n");
  printf("The above copyright notice and this permission notice shall be included in all\n");
  printf("copies or substantial portions of the Software.\n");
  printf("\n");
  printf("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n");
  printf("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n");
  printf("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n");
  printf("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n");
  printf("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n");
  printf("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n");
  printf("SOFTWARE.\n");
  printf("\n");
  printf("Compiled (64-bit) with libpcap, with POSIX capabilities (Linux)\n");
}



inline void usage(const char* progname)
{
  printf("Usage: %s [options] ... \n", progname);
  printf("\n");
  printf("OPTION:\n");
  printf("   -r <pcap file>        input interface is pcapfile\n");
  printf("   -i <interface>        input interface is network interface\n");
  printf("   -f <capture filter>   compile and use PCAP-CAPTHRE-FILTER \n");
  printf("   -a                    automaticaly to scroll\n");
  printf("   -v                    print version\n");
  printf("   -h                    show usage\n");
  printf("\n");
  printf("COMMANDS\n");
  printf("   j/k                   cursor down/up\n");
  printf("   <down>/<up>           cursor down/up. It also be able to use. Thx @epcnt19\n");
  printf("   <Tab>                 switch pane \n");
  printf("   <Space>               open/close element\n");
}






# cuishark
A protocol analyzer like wireshark on CUI/TUI

![screenshot](./imgs/ss1.png)


## Usage

```
$ git clone https://github.com/slankdev/cuishark.git
$ cd cuishark
$ make
$ sudo ./cuishark net eth0           // network-interface
$ sudo ./cuishark pcap input.pcap    // pcap-file-interface
```


## Requirement

 - libpcap
 - libncurses
 - libslankdev (https://github.com/slankdev/libslankdev)

```
$ sudo apt install libpcap-dev libncurses5
$ git clone https://github.com/slankdev/libslankdev.git
$ cd libslankdev
$ sudo make install
```


## License

MIT



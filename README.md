
# Cuishark

A protocol analyzer like wireshark on CUI/TUI

![screenshot](./imgs/ss1.png)

## Description

This is an implementation like a wireshark on CUI/TUI.  I think using Wireshark
on GUI is really troublesome. So, I wish that using Cuishark is effortless and,
speedy. Cuishark can be controlled by VI-like commands. (ex, hjkl)


## Usage

running ``cuishark`` prints Usage.  If you want to watch help, command ``?``
prints help message while running.

```
$ git clone https://github.com/slankdev/cuishark.git
$ cd cuishark
$ make
$ sudo ./cuishark net eth0           // network-interface
$ sudo ./cuishark pcap input.pcap    // pcap-file-interface
```


## Support Interface

 - pcap-file
 - network interface



## Requirement

 - libpcap
 - libncurses

We assume that users use Ubuntu. If you use another distribution, please change
Package-Manager according to the environment.

```
$ sudo apt install libpcap-dev libncurses5
```



## Version

 - v0.0 <---- This version
 - v0.1 <---- Developing now
    - support differential drawing
		- using ncurses-APIs more
 - v1.0 (Support Wireshark Display Filter)

I developed v0.0 with ncurses and c++11. However many implementation don't use
recommended-APIs of ncurses, using original-implementation. So, v0.1 will use
recommended-APIs of ncurses as much as possible.



## License

MIT



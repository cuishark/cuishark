

# Cuishark

A protocol analyzer like wireshark on CUI/TUI

**[Here is demo recording with asciinema.](https://asciinema.org/a/142181)**

![screenshot](./imgs/ss1.png)

## Description

This is an implementation like a wireshark on CUI/TUI.  I think using Wireshark
on GUI is really troublesome. So, I wish that using Cuishark is effortless and,
speedy. Cuishark can be controlled by VI-like commands. (ex, hjkl)

## Usage

running ``cuishark`` prints Usage.

```
$ git clone https://github.com/slankdev/cuishark.git
$ cd cuishark
$ make
$ sudo ./cuishark -i eth0                  // network-interface
$ sudo ./cuishark -r input.pcap            // pcap-file-interface
$ sudo ./cuishark -i eth0 -f "tcp port 80" // use pcap-capture-filter
```

If you like CuiShark, ``sudo make install`` to install to ``/usr/local/bin``.

```
$ sudo make install   // install
$ sudo make uninstall // uninstall
```

While running, user can use following commands.

| Command         | Description         |
|:---------------:|:-------------------:|
| j,&lt;down&gt;  | cursor down         |
| k,&lt;up&gt;    | cursor up           |
| &lt;Tab&gt;     | switch pane         |
| &lt;Space&gt;   | Open/Close element  |

## Support Interface

 - pcap-file
 - network interface
 - PCAP capture filter syntax

## Requirement

 - libpcap
 - libncurses
 - clang

We assume that users use Ubuntu. If you use another distribution, please change
Package-Manager according to the environment.

```
$ sudo apt install libpcap-dev libncurses5-dev clang
```

## Release

Current stable version is v0.1.

 - v0.1.1
   - support differential drawing
   - using ncurses-APIs more

I developed v0.0 with ncurses and c++11. However many implementation don't use
recommended-APIs of ncurses, using original-implementation. So, v0.1 will use
recommended-APIs of ncurses as much as possible.

## License

MIT. see LICENCE.


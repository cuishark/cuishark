
# Cuishark

A protocol analyzer like wireshark on CUI/TUI


## Support Interface

 - pcap-file
 - network interface
 - PCAP capture filter syntax



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



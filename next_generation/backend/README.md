
# Setup Libwireshark

install required package.
```
$ sudo apt install autoconf libtool libtool-bin byacc flex libgcrypt-dev libglib2.0-dev qt5-default qtcreator
$ git clone https://github.com/slankdev/wireshark
$ cd wireshark
$ ./autogen.sh
$ ./configure
$ make && sudo make install
```

install libwireshark and libwiretap
```
$ sudo apt install libwireshark-dev libwiretap-dev
```


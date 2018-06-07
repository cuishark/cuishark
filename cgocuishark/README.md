
# prototype

```
$ sudo apt install \
  autoconf build-essential libtool    \
  libtool libtool-bin libgcrypt-dev   \
  bison flex libgtk-3-dev  libqt4-dev \
  libpcap-dev
$ git clone https://github.com/cuishark/wireshark
$ cd wireshark
$ ./autogen.sh
$ ./configure \
  --enable-wireshark=false \
  --enable-editcap=false   \
  --enable-mergecap=false  \
  --enable-text2cap=false  \
  --enable-sharkd=false
$ make && sudo make install
$ cd ../
$ git clone https://github.com/cuishark/prototype
$ cd prototype
$ make -C ui
$ make
$ make run
```


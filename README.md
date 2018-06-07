
# Cuishark

![](./misc/cuishark.gif)

This is cuishark's versin 2.0 prototype.
The old version of cuishark was fullscrached and not using wireshark's dissector.
This verison can use the wireshark's dissector with cuishark's Rich UI.
You can analize many-many protocol's using JUST-CUI!!!.
Packet makes us very happy.

## Required Packages

- Install libcuishark (https://github.com/cuishark/libcuishark)
- Install Golang

## Install Cuishark

```
$ go get github.com/cuishark/cuishark
$ cd $GOPATH/src/github.com/cuishark/cuishark
$ git checkout -b proto-v0.2.0 origin/proto-v0.2.0
$ cd $GOPATH/src/github.com/cuishark/cuishark/cgocuishark && go install
$ cd $GOPATH/src/github.com/cuishark/cuishark/cuishark && go install
$ sudo cp $GOPATH/bin/cuishark /usr/local/bin
```

## Author and License

Author
- name: Hiroki Shirokura
- email: slank.dev@gmail.com
- twitter: @slankdev
- facebook: hiroki.shirokura

This software is developing under the GPL2.


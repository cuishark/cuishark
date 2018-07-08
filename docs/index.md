
![](./logo.png)

## What's Cuishark?

Cuishark is very-powerful protocol analyzer which has really rich UI.
And you can use such a rich UI for protocol analysis on just CUI without GUI.
Cuishark is using libwireshark for the protocol dissection, so you can
analyze many protocol and your network debug makes better with Cuishark.

## Cuishark makes our network debug more rapidly

![](./screenshot.png)

## Install Cuishark

```
$ git clone http://github.com/cuishark/libcuishark
$ cd libcuishark
$ ./bootstrap.sh
$ make && sudo make install
$ go get github.com/cuishark/cuishark
$ cd $GOPATH/src/github.com/cuishark/cuishark
$ cd $GOPATH/src/github.com/cuishark/cuishark/cgocuishark && go install
$ cd $GOPATH/src/github.com/cuishark/cuishark/cuishark && go install
$ sudo cp $GOPATH/bin/cuishark /usr/local/bin
```

While running, user can use following commands.

| Command         | Description         |
|:---------------:|:-------------------:|
| j,&lt;down&gt;  | cursor down         |
| k,&lt;up&gt;    | cursor up           |
| &lt;Tab&gt;     | switch pane         |
| &lt;Space&gt;   | Open/Close element  |

## Author and License

Author
- name: Hiroki Shirokura
- email: slank.dev@gmail.com
- twitter: @slankdev
- facebook: hiroki.shirokura

This software is developing under the GPL2.



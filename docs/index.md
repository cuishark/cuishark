
# Cuishark: Powerful protocol analyzer running on CUI

A protocol analyzer like wireshark on CUI/TUI

![](./cuishark.png)

## Install Cuishark

```
$ go get github.com/cuishark/cuishark
$ cd $GOPATH/src/github.com/cuishark/cuishark
$ git checkout -b proto-v0.2.0 origin/proto-v0.2.0
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



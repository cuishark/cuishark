
package main

import (
  "strings"
  "github.com/jroimartin/gocui"
  "github.com/cuishark/cuishark/cgocuishark"
)


func cmdexec_dfilter(filterstr string) {
  cgocuishark.ApplyDfilter(filterstr)
}


func commandexec(g *gocui.Gui, cmd string) error {
  args := strings.Split(cmd, " ")
  if args[0] == "quit" {
    return gocui.ErrQuit
  } else if args[0] == "df" {
    filterstr := ""
    for i:=1; i<len(args); i++ {
      filterstr = filterstr + args[i]
    }
    cmdexec_dfilter(filterstr)
    return nil
  }
  return nil
}





package main

import (
  "strings"
  "github.com/jroimartin/gocui"
  "github.com/cuishark/cuishark/cgocuishark"
)


func cmdexec_dfilter(g *gocui.Gui, filterstr string) {
  view := getpane_PacketList(g)
  view.Clear()

  cgocuishark.ApplyDfilter(filterstr)
  cgocuishark.PacketsDump()
}


func commandexec(g *gocui.Gui, cmd string) error {
  args := strings.Split(cmd, " ")
  if args[0] == "quit" {
    return gocui.ErrQuit
  } else if args[0] == "df" {
    filterstr := ""
    for i:=1; i<len(args); i++ {
      filterstr = filterstr + " " + args[i]
    }
    cmdexec_dfilter(g, filterstr)
    return nil
  }
  return nil
}




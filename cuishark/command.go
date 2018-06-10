
package main

import (
  "github.com/jroimartin/gocui"
)

func commandexec(g *gocui.Gui, cmd string) error {
  if cmd == "quit" {
    return gocui.ErrQuit
  }
  return nil
}




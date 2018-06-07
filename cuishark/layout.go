
package main

import (
  "fmt"
  "log"
  "github.com/jroimartin/gocui"
)


func layout(g *gocui.Gui) error {

  f := func(name string, x0, y0, x1, y1 int) {
    if v, err := g.SetView(name, x0, y0, x1, y1); err != nil {
      if err != gocui.ErrUnknownView {
        log.Panicln(name, err)
      }
      v.Frame = false
      v.Highlight = true
      v.SelFgColor = gocui.ColorDefault | gocui.AttrUnderline
    }
  }


  maxX, maxY := g.Size()
  h  := maxY
  w  := maxX
  ha := (h-1-1-2) % 3
  h1 := (h-1-1-2) / 3
  h2 := h1 + ha
  h3 := h2 + 2

  f("tb", -1, -1        , w, 1)
  f("v1", -1, 0        , w, h2)
  f("v2", -1, h2        , w, h2+h3)
  f("v3", -1, h2+h3     , w, h2+h3+h1-2)
  f("sb", -1, h2+h3+h1-3, w, h-1)
  f("cb", -1, h2+h3+h1-2, w, h)

  tb, _ := g.View("tb")
  tb.Clear()
  fmt.Fprintf(tb, "%-5s %-10s %-12s   %-12s %-5s %-5s %-80s",
    "idx", " time", "src", "dst", "proto", "len", "summary")
  tb.Highlight = true
  tb.SelFgColor = gocui.AttrReverse | gocui.AttrBold

  v2, _ := g.View("v2")
  v2.Frame = true

  vv, _ := g.View("sb")
  vv.Clear()
  vv.SelFgColor = gocui.AttrReverse | gocui.AttrBold
  str := "off"
  if auto_scroll { str = "on" }
  fmt.Fprintf(vv, "cuishark version 2.0 [if=%s] [auto_scroll=%s]%100s", ifname, str, "")

  cb, _ := g.View("cb")
  cb.Clear()
  fmt.Fprintf(cb, "commandframe %200s", "")

  if len(packets) > 0 {
    v1, _ := g.View("v1")
    _, oy := v1.Origin()
    _, y := v1.Cursor()
    idx := y + oy;
    show_packet_detail(g, idx)
  }
  return nil
}


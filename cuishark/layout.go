
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

  /*
   * Calculate Sizes
   */
  maxX, maxY := g.Size()
  h  := maxY
  w  := maxX
  ha := (h-1-1-2) % 3
  h1 := (h-1-1-2) / 3
  h2 := h1 + ha
  h3 := h2 + 2

  /*
   * Init Layout
   */
  f("TopStatusBar"   , -1, -1        , w, 1)
  f("PacketList"     , -1, 0         , w, h2)
  f("PacketDetail"   , -1, h2        , w, h2+h3)
  f("PacketByte"     , -1, h2+h3     , w, h2+h3+h1-2)
  f("BottomStatusBar", -1, h2+h3+h1-3, w, h-1)
  f("CommandBar"     , -1, h2+h3+h1-2, w, h)

  /*
   * Update Layout
   */
  update_TopStatusBar(g)
  update_PacketList(g)
  update_PacketDetail(g)
  update_BottomStatusBar(g)
  update_CommandBar(g)
  return nil
}

func update_TopStatusBar(g *gocui.Gui) {
  view, _ := g.View("TopStatusBar")
  view.Clear()
  fmt.Fprintf(view, "%-5s %-10s %-12s   %-12s %-5s %-5s %-80s",
    "idx", " time", "src", "dst", "proto", "len", "summary")
  view.Highlight = true
  view.SelFgColor = gocui.AttrReverse | gocui.AttrBold
}

func update_PacketList(g *gocui.Gui) {
  if first {
    _, err := g.SetCurrentView("PacketList")
    if err != nil {
      log.Panicln(err)
    }
    first = false
  }
}

func update_PacketDetail(g *gocui.Gui) {
  view, _ := g.View("PacketDetail")
  view.Frame = true
  if len(packets) > 0 {
    plist, _ := g.View("PacketList")
    _, oy := plist.Origin()
    _, y := plist.Cursor()
    idx := y + oy;
    show_packet_detail(g, idx)
  }
}

func update_BottomStatusBar(g *gocui.Gui) {
  view, _ := g.View("BottomStatusBar")
  view.Clear()
  view.SelFgColor = gocui.AttrReverse | gocui.AttrBold
  str := "off"
  if auto_scroll { str = "on" }
  fmt.Fprintf(view, "cuishark version 2.0 [if=%s] [auto_scroll=%s]%100s", ifname, str, "")
}

func update_CommandBar(g *gocui.Gui) {
  view, _ := g.View("CommandBar")
  view.Highlight = false
  view.Editable = true
}





package main

import (
  "log"
  "github.com/jroimartin/gocui"
)

func getpane_PacketList(g *gocui.Gui) (*gocui.View) {
  view, err := g.View("v1")
  if err != nil {
    log.Panicln(err)
  }
  return view
}

func getpane_PacketDetail(g *gocui.Gui) (*gocui.View) {
  view, err := g.View("v2")
  if err != nil {
    log.Panicln(err)
  }
  return view
}

func getpane_PacketByte(g *gocui.Gui) (*gocui.View) {
  view, err := g.View("v3")
  if err != nil {
    log.Panicln(err)
  }
  return view
}

func nextView(g *gocui.Gui, v *gocui.View) error {
  nextIndex := (active + 1) % len(viewArr)
  name := viewArr[nextIndex]
  if _, err := g.SetCurrentView(name); err != nil {
    return err
  }

  active = nextIndex
  return nil
}

func page_top(g *gocui.Gui, v *gocui.View) error {
  nextIndex := (active) % len(viewArr)
  name := viewArr[nextIndex]
  v2, err := g.View(name)
  if err != nil {
    return err
  }

  v2.SetOrigin(0, 0);
  v2.SetCursor(0, 0);
  return nil
}

func switch_auto_scroll(g *gocui.Gui, v *gocui.View) error {
  auto_scroll = !auto_scroll
  return nil
}

func page_bottom(g *gocui.Gui, v *gocui.View) error {
  nextIndex := (active) % len(viewArr)
  name := viewArr[nextIndex]
  v2, err := g.View(name)
  if err != nil {
    return err
  }

  _, height := v2.Size()
  _, y0 := v2.Origin()
  for ;; {
    for i:=0; i<height; i++ {
      v2.MoveCursor(0, 1, false);
    }

    _, y1 := v2.Origin()
    if (y1 == y0) {
      break
    } else {
      y0 = y1
    }
  }
  return nil
}

func page_up(g *gocui.Gui, v *gocui.View) error {
  nextIndex := (active) % len(viewArr)
  name := viewArr[nextIndex]
  v2, err := g.View(name)
  if err != nil {
    return err
  }

  _, y := v2.Size()
  for i:=0; i<y; i++ {
    v2.MoveCursor(0, -1, false);
  }
  return nil
}

func page_down(g *gocui.Gui, v *gocui.View) error {
  nextIndex := (active) % len(viewArr)
  name := viewArr[nextIndex]
  v2, err := g.View(name)
  if err != nil {
    return err
  }

  _, y := v2.Size()
  for i:=0; i<y; i++ {
    v2.MoveCursor(0, 1, false);
  }
  return nil
}

func scroll_up(g *gocui.Gui, v *gocui.View) error {
  nextIndex := (active) % len(viewArr)
  name := viewArr[nextIndex]
  v2, err := g.View(name)
  if err != nil {
    return err
  }

  v2.MoveCursor(0, -1, false);
  return nil
}

func scroll_PacketList(g *gocui.Gui, dy int) {
  view := getpane_PacketList(g)
  view.MoveCursor(0, dy, false);
}

func scroll_down(g *gocui.Gui, v *gocui.View) error {
  nextIndex := (active) % len(viewArr)
  name := viewArr[nextIndex]
  v2, err := g.View(name)
  if err != nil {
    return err
  }

  v2.MoveCursor(0, 1, false);
  return nil
}

func quit(g *gocui.Gui, v *gocui.View) error {
  return gocui.ErrQuit
}

func showdetail(g *gocui.Gui, v *gocui.View) error {
  view := getpane_PacketList(g)
  _, oy := view.Origin()
  _, y := view.Cursor()
  idx := y + oy;

  show_packet_detail(g, idx)
  return nil
}



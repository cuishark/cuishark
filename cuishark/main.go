package main

import (
  "fmt"
  "log"
  "os"
  "encoding/hex"
  "github.com/jroimartin/gocui"
  "github.com/cuishark/prototype/cgocuishark"
)

func main() {
  g, err := gocui.NewGui(gocui.OutputNormal)
  if err != nil {
    log.Panicln(err)
  }
  defer g.Close()

  g.Cursor = true
  g.Highlight = true
  g.SelFgColor = gocui.ColorGreen
  g.SetManagerFunc(layout)

  if err := g.SetKeybinding("", gocui.KeyCtrlA, gocui.ModNone, switch_auto_scroll); err != nil { log.Panicln(err) }
  if err := g.SetKeybinding("", 'g', gocui.ModNone, page_top); err != nil { log.Panicln(err) }
  if err := g.SetKeybinding("", 'G', gocui.ModNone, page_bottom); err != nil { log.Panicln(err) }
  if err := g.SetKeybinding("", gocui.KeyCtrlB, gocui.ModNone, page_up); err != nil { log.Panicln(err) }
  if err := g.SetKeybinding("", gocui.KeyCtrlF, gocui.ModNone, page_down); err != nil { log.Panicln(err) }
  if err := g.SetKeybinding("", gocui.KeyArrowUp, gocui.ModNone, scroll_up); err != nil { log.Panicln(err) }
  if err := g.SetKeybinding("", gocui.KeyArrowDown, gocui.ModNone, scroll_down); err != nil { log.Panicln(err) }
  if err := g.SetKeybinding("", 'j', gocui.ModNone, scroll_down); err != nil { log.Panicln(err) }
  if err := g.SetKeybinding("", 'k', gocui.ModNone, scroll_up); err != nil { log.Panicln(err) }
  if err := g.SetKeybinding("", gocui.KeyCtrlC, gocui.ModNone, quit); err != nil { log.Panicln(err) }
  if err := g.SetKeybinding("", gocui.KeyTab, gocui.ModNone, nextView); err != nil { log.Panicln(err) }
  if err := g.SetKeybinding("v2", gocui.KeySpace, gocui.ModNone, openclose_detail_pane); err != nil { log.Panicln(err) }

  go cgocuishark.Init(os.Args)
  go frontend_loop(g)

  if err := g.MainLoop(); err != nil && err != gocui.ErrQuit {
    log.Panicln(err)
  }
}


func frontend_loop(g *gocui.Gui) {

  packet_cnt := 1
  for cgocuishark.LoopRunning() {
    v := getpane_PacketList(g)
    ifname = cgocuishark.GetInterfaceName()

    if (!cgocuishark.MsgQueueEmpty()) {

      pkt := cgocuishark.MsgQueuePop()
      node := pkt.Node()
      fmt.Fprintf(v, "%s\n", node.Line())

      packets = append(packets, *pkt)
      if (packet_cnt == 1) {
        show_packet_detail(g, 0)
      }
      packet_cnt++

      if auto_scroll {
        _, height := v.Size()
        _, y0 := v.Origin()
        for ;; {
          for i:=0; i<height; i++ {
            v.MoveCursor(0, 1, false);
          }
          _, y1 := v.Origin()
          if (y1 == y0) {
            break
          } else {
            y0 = y1
          }
        }
      }

      g.Update(func(g *gocui.Gui) error { return nil })
    }
  }
  print("finish\n")

}

func print_csnode(view *gocui.View, node *cgocuishark.Node, level int) {
  gostr := node.Line()

  if (len(gostr) > 0) {
    for i:=0; i<level; i++ {
      fmt.Fprintf(view, "  ")
    }
    fmt.Fprintf(view, "%s\n", gostr)
  }

  if (node.IsOpen()) {
    for i:=0; i<node.ChildsNum(); i++ {
      child_node := node.Child(i);
      print_csnode(view, child_node, level+1);
    }
  }
}

func view_hexdump(view *gocui.View, slice []uint8) {
  dumper := hex.Dumper(view)
	defer dumper.Close()
  dumper.Write(slice)
}

func show_packet_detail(g *gocui.Gui, idx int) {
  if idx >= len(packets) {
    return
  }

  detail := getpane_PacketDetail(g)
  detail.Clear()
  pkt := packets[idx]
  node := pkt.Node()
  for i:=0; i<node.ChildsNum(); i++ {
    child_node := node.Child(i);
    print_csnode(detail, child_node, 0);
  }

  byte_pane := getpane_PacketByte(g)
  byte_pane.Clear()
  slice := pkt.Bytes()
  view_hexdump(byte_pane, slice)
}

func openclose_detail_pane(g *gocui.Gui, v *gocui.View) error {
  v1, _ := g.View("v1")
  _, v1_oy := v1.Origin()
  _, v1_y := v1.Cursor()
  v1_idx := v1_y + v1_oy;
  pkt := packets[v1_idx]
  root_node := pkt.Node()

  view, _ := g.View("v2")
  _, v2_oy := view.Origin()
  _, v2_y := view.Cursor()
  v2_idx := v2_y + v2_oy;

  cur_node := root_node.Search(v2_idx)
  cur_node.IsOpenSwitch()
  return nil
}


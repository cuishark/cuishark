package main

import (
  "fmt"
  "log"
  "os"
  "encoding/hex"
  "github.com/jroimartin/gocui"
  "github.com/cuishark/cuishark/cgocuishark"
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

  set_keybind(g, "", gocui.KeyCtrlA, gocui.ModNone, switch_auto_scroll)
  set_keybind(g, "", 'g', gocui.ModNone, page_top)
  set_keybind(g, "", 'G', gocui.ModNone, page_bottom)
  set_keybind(g, "", gocui.KeyCtrlB, gocui.ModNone, page_up)
  set_keybind(g, "", gocui.KeyCtrlF, gocui.ModNone, page_down)
  set_keybind(g, "", gocui.KeyArrowUp, gocui.ModNone, scroll_up)
  set_keybind(g, "", gocui.KeyArrowDown, gocui.ModNone, scroll_down)

  set_keybind(g, "PacketList", 'j', gocui.ModNone, scroll_down)
  set_keybind(g, "PacketList", 'k', gocui.ModNone, scroll_up)
  set_keybind(g, "PacketDetail", 'j', gocui.ModNone, scroll_down)
  set_keybind(g, "PacketDetail", 'k', gocui.ModNone, scroll_up)
  set_keybind(g, "PacketByte", 'j', gocui.ModNone, scroll_down)
  set_keybind(g, "PacketByte", 'k', gocui.ModNone, scroll_up)

  // set_keybind(g, "", gocui.KeyCtrlC, gocui.ModNone, quit)
  set_keybind(g, "", gocui.KeyTab, gocui.ModNone, nextView)
  set_keybind(g, "PacketDetail", gocui.KeySpace, gocui.ModNone, openclose_detail_pane)
  set_keybind(g, "", ':', gocui.ModNone, enterCmdMode)
  set_keybind(g, "CommandBar", gocui.KeyEnter, gocui.ModNone, CmdExec)

  go cgocuishark.Init(os.Args)
  go frontend_loop(g)

  if err := g.MainLoop(); err != nil && err != gocui.ErrQuit {
    log.Panicln(err)
  }
}

func set_keybind(g *gocui.Gui, viewname string,
         key interface{}, mod gocui.Modifier,
         handler func(*gocui.Gui, *gocui.View) error) {
  err := g.SetKeybinding(viewname, key, mod, handler)
  if err != nil {
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
  list, _ := g.View("PacketList")
  _, list_oy := list.Origin()
  _, list_y := list.Cursor()
  list_idx := list_y + list_oy;
  pkt := packets[list_idx]
  root_node := pkt.Node()

  detail, _ := g.View("PacketDetail")
  _, detail_oy := detail.Origin()
  _, detail_y := detail.Cursor()
  detail_idx := detail_y + detail_oy;
  cur_node := root_node.Search(detail_idx)
  cur_node.IsOpenSwitch()
  return nil
}

func enterCmdMode(g *gocui.Gui, v *gocui.View) error {
  view, err := g.SetCurrentView("CommandBar")
  if err != nil {
    log.Panicln(err)
  }
  view.Clear()
  fmt.Fprintf(view, ":")
  view.SetCursor(1, 0)
  return nil
}

func CmdExec(g *gocui.Gui, v *gocui.View) error {
  g.SetCurrentView("CommandBar")
  view, _ := g.View("CommandBar")
  cmdstr := view.Buffer()
  cmdstr = cmdstr[1:len(cmdstr)-1]
  view.Clear()
  fmt.Fprintf(view, "-EXEC-[%s]", cmdstr);
  g.SetCurrentView("PacketList")
  return commandexec(g, cmdstr)
}


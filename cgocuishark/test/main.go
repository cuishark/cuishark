
package main

import "fmt"
import "os"
import "bufio"
import "strings"
import "github.com/cuishark/cuishark/cgocuishark"

func frontend_loop() {

  for cgocuishark.LoopRunning() {

    if (!cgocuishark.MsgQueueEmpty()) {

      pkt := cgocuishark.MsgQueuePop()
      node := pkt.Node()
      fmt.Printf("%s\n", node.Line())

    }

  } /* for cgocuishark.LoopRunning() */
  print("finish\n")
}

func main() {
  go frontend_loop()

  go func() {
    cgocuishark.Init(os.Args)
    cgocuishark.Capture()
    // cgocuishark.Fini()
  }()

  for {
    fmt.Printf(">>> ")

    str := StrStdin()
    str = strings.TrimRight(str, "\n")

    args := strings.Split(str, " ")
    if str == "" {
      continue
    } else if args[0] == "quit" {
      break;
    } else if args[0] == "dump" {
      cgocuishark.StatusDump()
    } else if args[0] == "print" {
      cgocuishark.PacketsDump()
    } else if args[0] == "df" {
      cgocuishark.ApplyDfilter(args[1])
      cgocuishark.PacketsDump()
    } else {
      fmt.Printf("command not found: %s\n", str)
    }
  }
}


func StrStdin() (stringInput string) {
  scanner := bufio.NewScanner(os.Stdin)
  scanner.Scan()
  stringInput = scanner.Text()
  stringInput = strings.TrimSpace(stringInput)
  return
}

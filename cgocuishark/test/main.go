
package main

import "os"
import "fmt"
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

  cgocuishark.Init(os.Args)
  cgocuishark.Capture()
  cgocuishark.Fini()
}


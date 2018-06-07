
package main

/*
#cgo pkg-config: glib-2.0
#cgo CFLAGS: -I./lib
#cgo LDFLAGS: -L./lib -L/usr/local/lib
#cgo LDFLAGS: -lcuishark
#cgo LDFLAGS: -lwireshark -lgmodule-2.0
#cgo LDFLAGS: -pthread -lgthread-2.0 -pthread -lglib-2.0
#cgo LDFLAGS: -lwiretap -lwsutil -lz -lm -lpcap -lstdc++
#include <cuishark.h>
#include <hexdump.h>
*/
import "C"
import "os"
import "fmt"

func main() {
  argc := C.int(len(os.Args))
  argv := make([]*C.char, argc)
  for i, arg := range os.Args {
    argv[i] = C.CString(arg)
  }

  go func() {
    for C.cuishark_loop_running() {
      if (!C.cuishark_msg_queue_empty()) {

        m := C.cuishark_msgqueue_pop()
        n := C.cuishark_msg_node(m)

        s := C.GoString(C.node_line(n))
        fmt.Printf("%s\n", s)
        var i C.size_t
        for i=0; i<C.node_childs_num(n); i++ {
          cn := C.node_child(n, i);
          C.print_csnode(cn, 0);
        }
        C.hexdump(C.stdout, C.cuishark_msg_data_ptr(m), C.cuishark_msg_data_len(m));
        print("\n\n");

      }
    }
    print("finish\n")
  }()

  C.cuishark_init(argc, (**C.char)(&argv[0]))
}


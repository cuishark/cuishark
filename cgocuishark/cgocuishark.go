
package cgocuishark

/*
#cgo pkg-config: glib-2.0
#cgo CFLAGS: -I./lib
#cgo LDFLAGS: -L./lib
#cgo LDFLAGS: -L/usr/local/lib
#cgo LDFLAGS: -lcuishark
#cgo LDFLAGS: -lwireshark -lgmodule-2.0
#cgo LDFLAGS: -pthread -lgthread-2.0 -pthread -lglib-2.0
#cgo LDFLAGS: -lwiretap -lwsutil -lz -lm -lpcap -lstdc++
#include <cuishark.h>
*/
import "C"
import "bytes"
import "unsafe"
import "encoding/binary"


type Packet struct {
  raw *C.packet_t
}

type Node struct {
  raw *C.node_t
}

func (pkt *Packet) Bytes() []uint8 {
  array := C.cuishark_msg_data_ptr(pkt.raw)
  length := C.cuishark_msg_data_len(pkt.raw)

	slice := make([]uint8, int(length))
	b := C.GoBytes(unsafe.Pointer(array), C.int(length))
	err := binary.Read(bytes.NewReader(b), binary.LittleEndian, slice)
	if err != nil {
		panic(err)
	}

  return slice
}

func (pkt *Packet) Node() (*Node) {
  pkt_raw := pkt.raw
  node_raw := C.cuishark_msg_node(pkt_raw)
  return &Node{node_raw}
}

func (node *Node) ChildsNum() int {
  cint := C.node_childs_num(node.raw)
  return int(cint)
}

func (node *Node) Child(idx int) (*Node) {
  c_idx := C.ulong(idx)
  c_child_node := C.node_child(node.raw, c_idx)
  return &Node{c_child_node}
}

func (node *Node) IsOpen() bool {
  b := C.node_isopen(node.raw)
  return bool(b)
}

func (node *Node) IsOpenSwitch() {
  C.node_isopen_switch(node.raw)
}

func (node *Node) Search(idx int) *Node {
  i := (C.int)(idx)
  c := C.get_node_from_root(node.raw, i)
  return &Node{c}
}

func (node *Node) Line() string {
  s := C.GoString(C.node_line(node.raw))
  return s
}

func Init(args []string) {
  argc := C.int(len(args))
  argv := make([]*C.char, argc)
  for i, arg := range args {
    argv[i] = C.CString(arg)
  }

  C.cuishark_init(argc, (**C.char)(&argv[0]))
}

func Capture() {
  C.cuishark_capture()
}

func Fini() {
  C.cuishark_fini()
}

func ApplyDfilter(filter string) {
  cstr := C.CString(filter)
  C.cuishark_apply_dfilter(cstr)
}

func LoopRunning() bool {
  b := C.cuishark_loop_running()
  bb := bool(b)
  return bb
}

func MsgQueuePop() *Packet {
  pkt := C.cuishark_msgqueue_pop()
  gopkt := Packet{pkt}
  return &gopkt
}

func MsgQueueEmpty() bool {
  cb := C.cuishark_msg_queue_empty()
  return bool(cb)
}

func GetInterfaceName() string {
  cstr := C.get_interface_name()
  return C.GoString(cstr)
}

func PacketsDump() {
  C.cuishark_packets_dump()
}

func StatusDump() {
  C.cuishark_status_dump()
}



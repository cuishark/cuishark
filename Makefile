
SRC = main.go keyaction.go layout.go vars.go
TARGET = cuishark.v2.out

vlan:
	sudo go run $(SRC) -r ./misc/vlan.pcap

bgp:
	sudo go run $(SRC) -r ./misc/bgp.pcap

netif:
	sudo go run $(SRC) -i lo

build:
	rm -f $(TARGET)
	go build -o $(TARGET)

install: build
	cp $(TARGET) /usr/local/bin


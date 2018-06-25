
build:
	go build -o a.out *.go

pcap:
	go run *.go -r ../misc/bgp.pcap

net:
	sudo go run *.go -i lo

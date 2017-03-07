
#pragma once

#include <string>
#include <exception>
#include <sstream>
#include <ostream>

#include <rte_ring.h>
#include <rte_config.h>
#include <rte_version.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_ether.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <rte_hexdump.h>
#include <rte_ip.h>
#include <rte_ip_frag.h>

#include <slankdev/exception.h>

namespace slankdev {



class ether_addr : public ::ether_addr {
public:
    void print(FILE* fd) const
    {
        fprintf(fd, "%02" PRIx8 " %02" PRIx8 " %02" PRIx8
                   " %02" PRIx8 " %02" PRIx8 " %02" PRIx8,
                addr_bytes[0], addr_bytes[1],
                addr_bytes[2], addr_bytes[3],
                addr_bytes[4], addr_bytes[5]);
    }
    void sprint(char* str) const
    {
        sprintf(str, "%02" PRIx8 " %02" PRIx8 " %02" PRIx8
                   " %02" PRIx8 " %02" PRIx8 " %02" PRIx8,
                addr_bytes[0], addr_bytes[1],
                addr_bytes[2], addr_bytes[3],
                addr_bytes[4], addr_bytes[5]);
    }
};



class port_conf : public ::rte_eth_conf {
public:
    port_conf()
    {
        memset(this, 0, sizeof(rte_eth_conf));
        this->rxmode.max_rx_pkt_len = ETHER_MAX_LEN;
    }
};



class pool {
    rte_mempool* raw_;
public:
    pool() : raw_(nullptr) {}
    pool(const char* name,
            size_t nb_seg,
            size_t cache_siz,
            size_t mbuf_siz,
            uint16_t sock_id) : pool()
    {
        create(name,
            nb_seg, cache_siz,
            mbuf_siz, sock_id);
    }
    ~pool()
    {
        free();
    }
    void create(const char* name,
            size_t nb_seg,
            size_t cache_siz,
            size_t mbuf_siz,
            uint16_t sock_id)
    {
        if (raw_) {
            throw slankdev::exception("already created");
        }

        raw_ = rte_pktmbuf_pool_create(name,
                        nb_seg, cache_siz, 0,
                        mbuf_siz, sock_id);
        if (!raw_) {
            fprintf(stderr, "name     : %s  \n", name     );
            fprintf(stderr, "nb_seg   : %zd \n", nb_seg   );
            fprintf(stderr, "cache_siz: %zd \n", cache_siz);
            fprintf(stderr, "mbuf_siz : %zd \n", mbuf_siz );
            fprintf(stderr, "sock_id  : %u  \n", sock_id  );

            throw slankdev::exception("can not create pool");
        }
    }
    void free()
    {
        if (raw_)
            rte_mempool_free(raw_);
    }
    rte_mempool* get_raw() { return raw_; }
};



inline void dpdk_boot(int argc, char** argv)
{
    int ret = rte_eal_init(argc, argv);
    if (ret < 0)
        throw slankdev::exception("boot failed");
}



inline void port_init(uint16_t port, pool* mp, port_conf* conf,
        size_t nb_rx_rings, size_t nb_tx_rings,
        size_t rx_ring_size, size_t tx_ring_size)
{
	if (port >= rte_eth_dev_count())
        throw slankdev::exception("port is not exist");

	/*
     * Configure the Ethernet device.
     */
	int retval = rte_eth_dev_configure(port, nb_rx_rings, nb_tx_rings, conf);
	if (retval != 0)
        throw slankdev::exception("rte_eth_dev_configure failed");

	/*
     * Allocate and set up 1 RX queue per Ethernet port.
     */
	for (uint16_t q = 0; q < nb_rx_rings; q++) {
		retval = rte_eth_rx_queue_setup(port, q, rx_ring_size,
				rte_eth_dev_socket_id(port), NULL, mp->get_raw());
		if (retval < 0)
            throw slankdev::exception("rte_eth_rx_queue_setup failed");
	}

	/*
     * Allocate and set up 1 TX queue per Ethernet port.
     */
	for (uint16_t q = 0; q < nb_tx_rings; q++) {
		retval = rte_eth_tx_queue_setup(port, q, tx_ring_size,
				rte_eth_dev_socket_id(port), NULL);
		if (retval < 0)
            throw slankdev::exception("rte_eth_tx_queue_setup failed");
	}

	/*
     * Start the Ethernet port.
     */
	retval = rte_eth_dev_start(port);
	if (retval < 0)
        throw slankdev::exception("rte_eth_dev_start failed");

    /*
     * Enable Promiscous mode
     */
	rte_eth_promiscuous_enable(port);
}






}

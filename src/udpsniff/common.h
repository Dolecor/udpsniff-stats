/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>

#define PORTSTRLEN 6
#define MAX_PORT 65535
#define PACKET_MAX_LEN 65536

/* Values that represent any value in packet filtering */
#define ANY_IP INADDR_ANY
#define ANY_PORT (in_port_t)0

typedef struct packet_params {
    in_addr_t src_ip;
    in_addr_t dest_ip;
    in_port_t src_port;
    in_port_t dest_port;
} packet_params_t;

typedef struct statistics {
    size_t packets;
    size_t bytes;
} statistics_t;

int init_raw_socket(int *raw_socket, const char *netif, size_t netif_size);

int get_packet_params(const char *raw_packet, size_t size,
                      packet_params_t *params);

int check_packet_params(const char *raw_packet, size_t size,
                        const packet_params_t *filter);

#endif /* COMMON_H */

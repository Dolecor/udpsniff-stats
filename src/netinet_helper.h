/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef NETINET_HELPER_H
#define NETINET_HELPER_H

#include <netinet/in.h>

#include "common.h"

#define MAX_PORT 65535
#define PACKET_MAX_LEN 65536
#define PORTSTRLEN 6

/* Values that represent any value in packet filtering */
#define ANY_IP INADDR_ANY
#define ANY_PORT (in_port_t)0
#define ANY_STR "0"

int init_raw_socket(int *raw_socket, const char *netif, size_t netif_size);

int get_packet_params(const char *raw_packet, size_t size,
                      packet_params_t *params);
int check_packet_params(const char *raw_packet, size_t size,
                        const packet_params_t *filter);

void inaddr_to_str(in_addr_t ip, char *buf);
void port_to_str(in_port_t port, char *buf);

void str_to_inaddr(in_addr_t *ip, const char *buf);
void str_to_port(in_port_t *port, const char *buf);

#endif /* NETINET_HELPER_H */

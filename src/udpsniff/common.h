/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef UDPSNIFF_COMMON_H
#define UDPSNIFF_COMMON_H

#include <netinet/in.h>

#include "../common.h"

#define PORTSTRLEN 6
#define MAX_PORT 65535
#define PACKET_MAX_LEN 65536

/* Values that represent any value in packet filtering */
#define ANY_IP INADDR_ANY
#define ANY_PORT (in_port_t)0

int init_raw_socket(int *raw_socket, const char *netif, size_t netif_size);

int get_packet_params(const char *raw_packet, size_t size,
                      packet_params_t *params);

int check_packet_params(const char *raw_packet, size_t size,
                        const packet_params_t *filter);

#endif /* UDPSNIFF_COMMON_H */

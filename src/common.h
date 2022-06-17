/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef COMMON_H
#define COMMON_H

#include <netinet/in.h>

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

#endif /* COMMON_H */

/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#include "printstats.h"

#include <stdio.h>

#include "common.h"
#include "netinet_helper.h"

void printparams(packet_params_t params, const char *ifname)
{
#define DELIM_STR "-------------------------------------------"
    char src_ip[INET_ADDRSTRLEN];
    char dest_ip[INET_ADDRSTRLEN];
    char src_port[PORTSTRLEN];
    char dest_port[PORTSTRLEN];

    inaddr_to_str(params.src_ip, src_ip);
    inaddr_to_str(params.dest_ip, dest_ip);
    port_to_str(params.src_port, src_port);
    port_to_str(params.dest_port, dest_port);

    printf("Device name: %s\n", ifname);
    printf(DELIM_STR "\n");
    printf("%-15s %-5s %-15s %-5s\n", "source ip", "sport", "dest ip", "dport");
    printf("%-15s %-5s %-15s %-5s\n", src_ip, src_port, dest_ip, dest_port);
    printf(DELIM_STR "\n");
}

void printstats(statistics_t stats)
{
    printf("packets %ld bytes %ld\n", stats.packets, stats.bytes);
}

void printstats_cr(statistics_t stats)
{
    printf("\rpackets %ld bytes %ld", stats.packets, stats.bytes);
    fflush(stdout);
}

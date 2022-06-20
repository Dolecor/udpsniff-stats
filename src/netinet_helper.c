/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#include "netinet_helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

int init_raw_socket(int *raw_socket, const char *netif, size_t netif_size)
{
    *raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (*raw_socket < 0) {
        return 0;
    }

    if (setsockopt(*raw_socket, SOL_SOCKET, SO_BINDTODEVICE, netif, netif_size)
        == -1) {
        return 0;
    }

    if (fcntl(*raw_socket, F_SETFL, O_NONBLOCK) == -1) {
        perror("fcntl");
        return 0;
    }

    return 1;
}

int get_packet_params(const char *raw_packet, size_t size,
                      packet_params_t *params)
{
    const struct iphdr *iph = (const struct iphdr *)raw_packet;
    params->src_ip = iph->saddr;
    params->dest_ip = iph->daddr;

    const struct udphdr *udph =
        (const struct udphdr *)(raw_packet + sizeof(struct iphdr));
    params->src_port = udph->uh_sport;
    params->dest_port = udph->uh_dport;

    return 1;
}

int check_packet_params(const char *raw_packet, size_t size,
                        const packet_params_t *filter)
{
#define SRC_IP params.src_ip
#define DEST_IP params.dest_ip
#define SRC_PORT params.src_port
#define DEST_PORT params.dest_port

    packet_params_t params;
    get_packet_params(raw_packet, size, &params);

    return ((SRC_IP == filter->src_ip) || (filter->src_ip == ANY_IP))
           && ((DEST_IP == filter->dest_ip) || (filter->dest_ip == ANY_IP))
           && ((SRC_PORT == filter->src_port) || (filter->src_port == ANY_PORT))
           && ((DEST_PORT == filter->dest_port)
               || (filter->dest_port == ANY_PORT));
}

void inaddr_to_str(in_addr_t ip, char *buf)
{
    if (ip == ANY_IP) {
        strcpy(buf, ANY_STR);
    } else {
        inet_ntop(AF_INET, &ip, buf, INET_ADDRSTRLEN);
    }
}

void port_to_str(in_port_t port, char *buf)
{
    if (port == ANY_PORT) {
        strcpy(buf, ANY_STR);
    } else {
        sprintf(buf, "%d", port);
    }
}

void str_to_inaddr(in_addr_t *ip, const char *buf)
{
    if (buf[0] == ANY_STR[0]) {
        *ip = ANY_IP;
    } else {
        *ip = inet_addr(buf);
    }
}

void str_to_port(in_port_t *port, const char *buf)
{
    if (buf[0] == ANY_STR[0]) {
        *port = ANY_PORT;
    } else {
        *port = (uint16_t)strtol(buf, NULL, 0);
    }
}

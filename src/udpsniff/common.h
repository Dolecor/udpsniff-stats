#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>

#include <pthread.h>

typedef struct packet_info {
    in_addr_t src_ip;
    in_addr_t dest_ip;
    in_port_t src_port;
    in_port_t dest_port;
} packet_info_t;

typedef struct statistics {
    /* packet_info_t packet_info; */
    size_t packets;
    size_t bytes;
} statistics_t;

int init_raw_socket(int *raw_socket, const char *netif, size_t netif_size);

int get_packet_fields(const char *raw_packet, size_t size,
                      packet_info_t *fields);

int check_packet_fields(const char *raw_packet, size_t size,
                        const packet_info_t *params);

#endif /* COMMON_H */

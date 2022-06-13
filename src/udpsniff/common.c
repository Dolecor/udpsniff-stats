#include <netinet/ip.h>
#include <netinet/udp.h>

#include "common.h"

int init_raw_socket(int *raw_socket, const char *netif, size_t netif_size)
{
    int ret;

    *raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (*raw_socket < 0) {
        return EXIT_FAILURE;
    }

    ret =
        setsockopt(*raw_socket, SOL_SOCKET, SO_BINDTODEVICE, netif, netif_size);
    if (ret < 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int get_packet_params(const char *raw_packet, size_t size,
                      packet_params_t *params)
{
    // TODO: check raw_packet, params for NULL

    const struct iphdr *iph = (const struct iphdr *)raw_packet;
    params->src_ip = iph->saddr;
    params->dest_ip = iph->daddr;

    const struct udphdr *udph =
        (const struct udphdr *)(raw_packet + sizeof(struct iphdr));
    params->src_port = udph->uh_sport;
    params->dest_port = udph->uh_dport;

    return EXIT_SUCCESS;
}

int check_packet_params(const char *raw_packet, size_t size,
                        const packet_params_t *filter)
{
    packet_params_t params;
    get_packet_params(raw_packet, size, &params);

#define SRC_IP params.src_ip
#define DEST_IP params.dest_ip
#define SRC_PORT params.src_port
#define DEST_PORT params.dest_port

    return ((SRC_IP == filter->src_ip) || (filter->src_ip == ANY_IP))
           && ((DEST_IP == filter->dest_ip) || (filter->dest_ip == ANY_IP))
           && ((SRC_PORT == filter->src_port) || (filter->src_port == ANY_PORT))
           && ((DEST_PORT == filter->dest_port)
               || (filter->dest_port == ANY_PORT));
}

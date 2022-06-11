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

int get_packet_fields(const char *raw_packet, size_t size,
                      packet_info_t *fields)
{
    // TODO: check raw_packet, fields for NULL

    const struct iphdr *iph = (const struct iphdr *)raw_packet;
    fields->dest_ip = iph->daddr;
    fields->src_ip = iph->saddr;

    const struct udphdr *udph =
        (const struct udphdr *)(raw_packet + sizeof(struct iphdr));
    fields->src_port = udph->uh_sport;
    fields->dest_port = udph->uh_dport;

    return EXIT_SUCCESS;
}

/*  #include <arpa/inet.h>
 *    packet_info_t tmp;
 *    char ip_str[INET_ADDRSTRLEN];
 *
 *    get_packet_fields(raw_packet, BUFF_SIZE, &tmp);
 *
 *    printf("src_ip  : %s\n",
 *            inet_ntop(AF_INET, &tmp.src_ip, ip_str, INET_ADDRSTRLEN));
 *    printf("dest_ip : %s\n",
 *            inet_ntop(AF_INET, &tmp.dest_ip, ip_str, INET_ADDRSTRLEN));
 *    printf("\n");
 *
 *    printf("src_port  : %d\n", ntohs(tmp.src_port));
 *    printf("dest_port : %d\n", ntohs(tmp.dest_port));
 */

int check_packet_fields(const char *raw_packet, size_t size,
                        const packet_info_t *params)
{
    // TODO:
    // return 1 if raw_packet contains params
    // 0 otherwise

    return -1;
}

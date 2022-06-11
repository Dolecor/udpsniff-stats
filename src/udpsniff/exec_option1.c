#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/ip.h>

#include "common.h"

static struct thread_ret {
    int exit_status;
} sniffer_ret, provider_ret;

static int pipe_fds[2];

static void *sniff_packets()
{
    int ret;

    int sock;
    // struct sockaddr_in saddr;
    // socklen_t addr_len = sizeof(saddr);
    const char if_name[IF_NAMESIZE] = "lo\0";

    printf("%s\n", if_name);

    const size_t BUFF_SIZE = 65536;
    unsigned char raw_packet[BUFF_SIZE];
    ssize_t num_bytes;

    size_t packet_count = 0;
    statistics_t tmp;

    sniffer_ret.exit_status = EXIT_SUCCESS;

    ret = init_raw_socket(&sock, if_name, IF_NAMESIZE);
    if (ret) {
        perror("init_raw_socket");
        sniffer_ret.exit_status = EXIT_FAILURE;
        goto sniffer_exit;
    }

    while (1) {
        num_bytes = recvfrom(sock, raw_packet, BUFF_SIZE, 0, NULL, NULL
                             /* (struct sockaddr *)&saddr, &addr_len */);
        if (num_bytes == -1) {
            perror("recvfrom");
            sniffer_ret.exit_status = EXIT_FAILURE;
            goto sniffer_exit;
        }

        // TODO: check_packet_fields

        tmp.packets = 1;
        tmp.bytes = num_bytes;
        /* ntohs(((struct iphdr *)raw_packet)->tot_len); */

        if (write(pipe_fds[1], &tmp, sizeof(tmp)) < 0) {
            perror("write");
            sniffer_ret.exit_status = EXIT_FAILURE;
            goto sniffer_exit;
        }

        packet_count += 1;

        /// !!!!
        if (packet_count == 10) {
            sniffer_ret.exit_status = EXIT_SUCCESS;
            close(pipe_fds[1]); /* EOF */
            goto sniffer_exit;
        }
    }

sniffer_exit:
    close(sock);
    pthread_exit((void *)&sniffer_ret);
}

static void *provide_stats()
{
    ssize_t num_bytes;
    statistics_t tmp;
    statistics_t stat;

    provider_ret.exit_status = EXIT_SUCCESS;

    while (1) {
        num_bytes = read(pipe_fds[0], &tmp, sizeof(tmp));
        if (num_bytes < 0) {
            perror("write");
            provider_ret.exit_status = EXIT_FAILURE;
            goto provider_exit;
        } else if (num_bytes == 0) { /* EOF */
            provider_ret.exit_status = EXIT_SUCCESS;
            goto provider_exit;
        }

        stat.packets += 1;
        stat.bytes += tmp.bytes;

        /// !!!!
        printf("bytes : %ld    packets : %ld\n", stat.bytes, stat.packets);
    }

provider_exit:
    pthread_exit((void *)&provider_ret);
}

int exec_option1()
{
    pthread_t thr_sniffer;
    pthread_t thr_provider;
    struct thread_ret *thr_rets[2];
    int ret;

    if (pipe(pipe_fds) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    ret = pthread_create(&thr_sniffer, NULL, sniff_packets, NULL);
    if (ret != 0) {
        perror("pthread_create(thr_sniffer)");
        return EXIT_FAILURE;
    }

    ret = pthread_create(&thr_provider, NULL, provide_stats, NULL);
    if (ret != 0) {
        perror("pthread_create(thr_provider)");
        return EXIT_FAILURE;
    }

    ret = pthread_join(thr_sniffer, (void *)&thr_rets[0]);
    if (ret != 0) {
        perror("pthread_join(thr_sniffer)");
        return EXIT_FAILURE;
    }

    ret = pthread_join(thr_provider, (void *)&thr_rets[1]);
    if (ret != 0) {
        perror("pthread_join(thr_provider)");
        return EXIT_FAILURE;
    }

    printf("Sniffer exit status  : %d\n", thr_rets[0]->exit_status);
    printf("Provider exit status : %d\n", thr_rets[1]->exit_status);

    return EXIT_SUCCESS;
}

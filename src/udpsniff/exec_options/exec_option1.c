#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/ip.h>

#include "common.h"

struct thread_ret {
    int exit_status;
};

struct sniffer_arg {
    const char *if_name;
    packet_params_t packet_filter;
};

static int pipe_fds[2];

static void *sniff_packets(void *arg)
{
    struct sniffer_arg args = *((struct sniffer_arg *)arg);
    struct thread_ret ret;
    int sock;
    unsigned char raw_packet[PACKET_MAX_LEN];
    ssize_t num_bytes;
    statistics_t tmp;

    ret.exit_status = EXIT_SUCCESS;

    if (init_raw_socket(&sock, args.if_name, IF_NAMESIZE) == EXIT_FAILURE) {
        perror("init_raw_socket");
        ret.exit_status = EXIT_FAILURE;
        goto sniffer_exit;
    }

    while (1) {
        num_bytes = recvfrom(sock, raw_packet, PACKET_MAX_LEN, 0, NULL, NULL);
        if (num_bytes == -1) {
            perror("recvfrom");
            ret.exit_status = EXIT_FAILURE;
            goto sniffer_exit;
        }

        if (check_packet_params(raw_packet, num_bytes, &args.packet_filter)) {
            tmp.packets = 1;
            tmp.bytes = num_bytes;

            if (write(pipe_fds[1], &tmp, sizeof(tmp)) < 0) {
                perror("write");
                ret.exit_status = EXIT_FAILURE;
                goto sniffer_exit;
            }
        }
    }

sniffer_exit:
    close(sock);
    pthread_exit((void *)&ret);
}

static void *provide_stats(void *arg)
{
    const packet_params_t packet_info = *((const packet_params_t *)arg);
    struct thread_ret ret;
    ssize_t num_bytes;
    statistics_t tmp;
    statistics_t stat;

    ret.exit_status = EXIT_SUCCESS;

    while (1) {
        num_bytes = read(pipe_fds[0], &tmp, sizeof(tmp));
        if (num_bytes < 0) {
            perror("write");
            ret.exit_status = EXIT_FAILURE;
            goto provider_exit;
        } else if (num_bytes == 0) { /* EOF */
            ret.exit_status = EXIT_SUCCESS;
            goto provider_exit;
        }

        stat.packets += 1;
        stat.bytes += tmp.bytes;

        /// !!!!
        printf("bytes : %ld    packets : %ld\r", stat.bytes, stat.packets);
        fflush(stdout);
    }

provider_exit:
    pthread_exit((void *)&ret);
}

int exec_option1(const char *if_name, packet_params_t packet_info)
{
    pthread_t thr_sniffer;
    pthread_t thr_provider;
    struct thread_ret *thr_rets[2];
    int ret;

    struct sniffer_arg sniff_arg = {.if_name = if_name,
                                    .packet_filter = packet_info};
    packet_params_t provider_arg = packet_info;

    if (pipe(pipe_fds) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    ret = pthread_create(&thr_sniffer, NULL, sniff_packets, &sniff_arg);
    if (ret != 0) {
        perror("pthread_create(thr_sniffer)");
        return EXIT_FAILURE;
    }

    ret = pthread_create(&thr_provider, NULL, provide_stats, &provider_arg);
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

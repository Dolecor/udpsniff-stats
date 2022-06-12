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

static statistics_t stat = {.packets = 0, .bytes = 0};
static pthread_mutex_t mtx_stat = PTHREAD_MUTEX_INITIALIZER;

static void *sniff_packets(void *arg)
{
    struct sniffer_arg args = *((struct sniffer_arg *)arg);
    struct thread_ret ret;
    int sock;
    unsigned char raw_packet[PACKET_MAX_LEN];
    ssize_t num_bytes;
    statistics_t local_stat = {.packets = 0, .bytes = 0};
    int s;

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
            local_stat.packets += 1;
            local_stat.bytes += num_bytes;

            s = pthread_mutex_lock(&mtx_stat);
            if (s != 0) {
                ret.exit_status = EXIT_FAILURE;
                goto sniffer_exit;
            }

            stat = local_stat;

            s = pthread_mutex_unlock(&mtx_stat);
            if (s != 0) {
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
    statistics_t local_stat;
    int s;

    ret.exit_status = EXIT_SUCCESS;

    while (1) {
        sleep(1);

        s = pthread_mutex_lock(&mtx_stat);
        if (s != 0) {
            ret.exit_status = EXIT_FAILURE;
            goto provider_exit;
        }

        local_stat = stat;

        s = pthread_mutex_unlock(&mtx_stat);
        if (s != 0) {
            ret.exit_status = EXIT_FAILURE;
            goto provider_exit;
        }

        /// !!!!
        printf("bytes : %ld    packets : %ld\r", local_stat.bytes,
               local_stat.packets);
        fflush(stdout);
    }

provider_exit:
    pthread_exit((void *)&ret);
}

int exec_option2(const char *if_name, packet_params_t packet_info)
{
    pthread_t thr_sniffer;
    pthread_t thr_provider;
    struct thread_ret *thr_rets[2];
    int ret;

    struct sniffer_arg sniff_arg = {.if_name = if_name,
                                    .packet_filter = packet_info};
    packet_params_t provider_arg = packet_info;

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

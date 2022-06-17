/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.
 */

#include "exec_options.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <net/if.h>
#include <sys/socket.h>

#include "udpsniff/common.h"
#include "priv_exec_option1.h"
#include "priv_exec_option2.h"

typedef int (*sniffer_update_stat_cb)(size_t bytes);
typedef int (*provider_retrieve_stat_cb)(statistics_t *stat);
typedef int (*provider_send_stat_cb)();

typedef struct exec_option_config {
    int (*init_exec)();
    void (*free_exec)();
    sniffer_update_stat_cb update_cb;
    provider_retrieve_stat_cb retrieve_cb;
    provider_send_stat_cb send_cb;
} exec_option_config_t;

struct thread_ret {
    int exit_status;
};

struct sniffer_arg {
    const char *if_name;
    packet_params_t packet_filter;
    sniffer_update_stat_cb update_stat_cb;
};

struct provider_arg {
    packet_params_t packet_info;
    provider_retrieve_stat_cb retrieve_stat_cb;
};

static void *sniff_packets(void *arg)
{
    struct sniffer_arg args = *((struct sniffer_arg *)arg);
    struct thread_ret *ret =
        (struct thread_ret *)malloc(sizeof(struct thread_ret));
    int sock;
    unsigned char raw_packet[PACKET_MAX_LEN];
    ssize_t num_bytes;

    ret->exit_status = EXIT_SUCCESS;

    if (init_raw_socket(&sock, args.if_name, IF_NAMESIZE) == EXIT_FAILURE) {
        perror("init_raw_socket");
        ret->exit_status = EXIT_FAILURE;
        goto sniffer_exit;
    }

    while (1) {
        num_bytes = recvfrom(sock, raw_packet, PACKET_MAX_LEN, 0, NULL, NULL);
        if (num_bytes == -1) {
            perror("recvfrom");
            ret->exit_status = EXIT_FAILURE;
            goto sniffer_exit;
        }

        if (check_packet_params(raw_packet, num_bytes, &args.packet_filter)) {
            if (args.update_stat_cb(num_bytes) == EXIT_FAILURE) {
                ret->exit_status = EXIT_FAILURE;
                goto sniffer_exit;
            }
        }
    }

sniffer_exit:
    close(sock);
    pthread_exit((void *)ret);
}

static void *provide_stats(void *arg)
{
    struct provider_arg args = *((struct provider_arg *)arg);
    struct thread_ret *ret =
        (struct thread_ret *)malloc(sizeof(struct thread_ret));
    statistics_t stat;

    ret->exit_status = EXIT_SUCCESS;

    while (1) {
        if (args.retrieve_stat_cb(&stat) == EXIT_FAILURE) {
            ret->exit_status = EXIT_FAILURE;
            goto provider_exit;
        }
        /// !!!!
        printf("bytes : %ld    packets : %ld\r", stat.bytes, stat.packets);
        fflush(stdout);
    }

provider_exit:
    pthread_exit((void *)ret);
}

static int execute(const exec_option_config_t *config, const char *if_name,
                   packet_params_t packet_params)
{
    pthread_t thr_sniffer;
    pthread_t thr_provider;
    struct thread_ret *thr_rets[2];
    int ret;
    struct sniffer_arg sniff_arg = {.if_name = if_name,
                                    .packet_filter = packet_params,
                                    .update_stat_cb = config->update_cb};
    struct provider_arg prov_arg = {.packet_info = packet_params,
                                    .retrieve_stat_cb = config->retrieve_cb};

    if (config->init_exec() == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    ret = pthread_create(&thr_sniffer, NULL, sniff_packets, &sniff_arg);
    if (ret != 0) {
        perror("pthread_create(thr_sniffer)");
        return EXIT_FAILURE;
    }

    ret = pthread_create(&thr_provider, NULL, provide_stats, &prov_arg);
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

    free(thr_rets[0]);
    free(thr_rets[1]);
    config->free_exec();

    return EXIT_SUCCESS;
}

int exec_option(exec_option_t option, const char *if_name,
                packet_params_t packet_info)
{
    exec_option_config_t config;
    int ret;

    switch (option) {
    case OPT1:
        config.init_exec = init_exec_option1;
        config.free_exec = free_exec_option1;
        config.update_cb = sniffer_update_stat1;
        config.retrieve_cb = provider_retrieve_stat1;
        // config.send_cb
        break;
    case OPT2:
        config.init_exec = init_exec_option2;
        config.free_exec = free_exec_option2;
        config.update_cb = sniffer_update_stat2;
        config.retrieve_cb = provider_retrieve_stat2;
        // config.send_cb
        break;
    default:
        fprintf(stderr, "Wrong execution option\n");
        return EXIT_FAILURE;
    }

    ret = execute(&config, if_name, packet_info);

    return ret;
}

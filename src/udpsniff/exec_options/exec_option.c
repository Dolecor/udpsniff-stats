/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#include "exec_options.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <net/if.h>
#include <sys/socket.h>
#include <errno.h>

#include "netinet_helper.h"
#include "udpsniff/exec_options/priv_exec_option1.h"
#include "udpsniff/exec_options/priv_exec_option2.h"
#include "udpsniff/control.h"

#define USE_UBUS 1

#if USE_UBUS
#    include <libubus.h>
#    include <libubox/uloop.h>
#    include <libubox/blobmsg_json.h>
#    include "ipc/ubus/ubus_binary_helper.h"
#    include "ipc/ubus/ubus_common.h"
#else
#    include "ipc/mq/mq_interface_provider.h"
#endif

static struct thread_ret {
    int exit_status;
} sniff_ret, prov_ret;

struct sniffer_arg {
    const char *if_name;
    packet_params_t packet_filter;
    sniffer_update_stat_cb update_stat_cb;
};

struct provider_arg {
    const char *if_name;
    packet_params_t packet_info;
    provider_retrieve_stat_cb retrieve_stat_cb;
};

static void *sniff_packets(void *arg)
{
    struct sniffer_arg args = *((struct sniffer_arg *)arg);
    int sock;
    char raw_packet[PACKET_MAX_LEN];
    ssize_t num_bytes;

    sniff_ret.exit_status = EXIT_SUCCESS;

    if (!init_raw_socket(&sock, args.if_name, IF_NAMESIZE)) {
        perror("init_raw_socket");
        sniff_ret.exit_status = EXIT_FAILURE;
        goto sniffer_exit;
    }

    while (!stop_flag) {
        errno = 0;
        num_bytes = recvfrom(sock, raw_packet, PACKET_MAX_LEN, 0, NULL, NULL);
        if ((num_bytes == -1) && (errno != EAGAIN)) {
            perror("recvfrom");
            sniff_ret.exit_status = EXIT_FAILURE;
            goto sniffer_exit;
        } else if (errno == EAGAIN) {
            continue;
        }

        if (check_packet_params(raw_packet, num_bytes, &args.packet_filter)) {
            if (args.update_stat_cb(num_bytes) == RCEXEC_BROKEN_IPC) {
                sniff_ret.exit_status = EXIT_FAILURE;
                goto sniffer_exit;
            }
        }
    }

sniffer_exit:
    close(sock);
    if (sniff_ret.exit_status == EXIT_FAILURE) {
        raise(SIGTERM);
    }
    pthread_exit((void *)&sniff_ret);
}

#if USE_UBUS
static struct provider_arg prov_args;
static statistics_t loc_stats = {.packets = 0, .bytes = 0};
static msg_reply_t reply;

static struct ubus_context *ctx;
static struct ubus_subscriber test_event;
static struct blob_buf b;

enum {
    STATS_GET,
    __STATS_MAX
};

static const struct blobmsg_policy stats_policy[__STATS_MAX] = {
    [STATS_GET] = {.name = "get", .type = BLOBMSG_TYPE_UNSPEC}};

static int stats_get(struct ubus_context *ctx, struct ubus_object *obj,
                     struct ubus_request_data *req, const char *method,
                     struct blob_attr *msg)
{
    struct blob_attr *tb[__STATS_MAX];

    blobmsg_parse(stats_policy, __STATS_MAX, tb, blob_data(msg), blob_len(msg));
    if (!tb[STATS_GET])
        return UBUS_STATUS_INVALID_ARGUMENT;

    reply.stats = loc_stats;

    blob_buf_init(&b, 0);
    blobmsg_add_binary(&b, "stats", (char *)&reply, sizeof(msg_reply_t));
    ubus_send_reply(ctx, req, b.head);

    return 0;
}

static const struct ubus_method stats_methods[] = {
    UBUS_METHOD("get", stats_get, stats_policy)};

static struct ubus_object_type stats_object_type =
    UBUS_OBJECT_TYPE("stats", stats_methods);

static struct ubus_object stats_object = {
    .name = "stats",
    .type = &stats_object_type,
    .methods = stats_methods,
    .n_methods = ARRAY_SIZE(stats_methods),
};

#    define TIMEOUT_MSEC 100

static void retrieve_stat_cb(struct uloop_timeout *timeout)
{
    if (prov_args.retrieve_stat_cb(&loc_stats) == RCEXEC_BROKEN_IPC) {
        prov_ret.exit_status = EXIT_FAILURE;
        return;
    }

    //printf("%ld %ld\n", loc_stats.packets, loc_stats.bytes);

    uloop_timeout_set(timeout, TIMEOUT_MSEC);
}

static struct uloop_timeout retrieve = {.cb = retrieve_stat_cb};

static void *provide_stats(void *arg)
{
    prov_args = *((struct provider_arg *)arg);
    const char *ubus_socket = NULL;
    int ret;

    prov_ret.exit_status = EXIT_SUCCESS;

    strncpy(reply.ifname, prov_args.if_name, IF_NAMESIZE);
    reply.params = prov_args.packet_info;

    uloop_init();

    ctx = ubus_connect(ubus_socket);
    if (!ctx) {
        fprintf(stderr, "Failed to connect to ubus\n");
        prov_ret.exit_status = EXIT_FAILURE;
        goto provider_exit;
    }

    ubus_add_uloop(ctx);

    ret = ubus_add_object(ctx, &stats_object);
    if (ret) {
        fprintf(stderr,
                "Failed to add object: %s. Maybe udp-sniff is already running.\n",
                ubus_strerror(ret));
        prov_ret.exit_status = EXIT_FAILURE;
        goto provider_exit;
    }

    uloop_timeout_set(&retrieve, TIMEOUT_MSEC);

    uloop_run();

provider_exit:
    ubus_free(ctx);
    uloop_done();
    if (prov_ret.exit_status == EXIT_FAILURE) {
        raise(SIGTERM);
    }
    pthread_exit((void *)&prov_ret);
}
#else
static void *provide_stats(void *arg)
{
#    define MAX_LOOP_CNT 10
    struct provider_arg args = *((struct provider_arg *)arg);
    statistics_t stat;

    prov_ret.exit_status = EXIT_SUCCESS;

    if (!init_mq_prov(args.packet_info, args.if_name)) {
        prov_ret.exit_status = EXIT_FAILURE;
        goto provider_exit;
    }

    while (!stop_flag) {
        if (args.retrieve_stat_cb(&stat) == RCEXEC_BROKEN_IPC) {
            prov_ret.exit_status = EXIT_FAILURE;
            goto provider_exit;
        }

        int loop_cnt = 0;
        while (check_request() && (loop_cnt < MAX_LOOP_CNT)) {
            send_reply(args.packet_info, stat, args.if_name);
            ++loop_cnt;
        }
    }

provider_exit:
    free_mq_prov();
    if (prov_ret.exit_status == EXIT_FAILURE) {
        raise(SIGTERM);
    }
    pthread_exit((void *)&prov_ret);
}
#endif

static int execute(const exec_option_config_t *config, const char *if_name,
                   packet_params_t packet_params)
{
    int ret = EXIT_SUCCESS;
    pthread_t thr_sniffer;
    pthread_t thr_provider;
    struct thread_ret *thr_rets[2];
    struct sniffer_arg sniff_arg = {.if_name = if_name,
                                    .packet_filter = packet_params,
                                    .update_stat_cb = config->update_cb};
    struct provider_arg prov_arg = {.if_name = if_name,
                                    .packet_info = packet_params,
                                    .retrieve_stat_cb = config->retrieve_cb};

    sniff_arg.packet_filter.src_port = htons(sniff_arg.packet_filter.src_port);
    sniff_arg.packet_filter.dest_port =
        htons(sniff_arg.packet_filter.dest_port);

    if (!set_signals()) {
        ret = EXIT_FAILURE;
        goto exit;
    }

    if (config->init_exec() != RCEXEC_OK) {
        ret = EXIT_FAILURE;
        goto err_free;
    }

    errno = pthread_create(&thr_sniffer, NULL, sniff_packets, &sniff_arg);
    if (errno != 0) {
        perror("pthread_create(thr_sniffer)");
        ret = EXIT_FAILURE;
        goto err_free;
    }

    errno = pthread_create(&thr_provider, NULL, provide_stats, &prov_arg);
    if (errno != 0) {
        perror("pthread_create(thr_provider)");
        ret = EXIT_FAILURE;
        goto err_free;
    }

    errno = pthread_join(thr_sniffer, (void *)&thr_rets[0]);
    if (errno != 0) {
        perror("pthread_join(thr_sniffer)");
        ret = EXIT_FAILURE;
        goto err_free;
    }

    errno = pthread_join(thr_provider, (void *)&thr_rets[1]);
    if (errno != 0) {
        perror("pthread_join(thr_provider)");
        ret = EXIT_FAILURE;
        goto err_free;
    }

    printf("Sniffer exit status  : %s\n",
           thr_rets[0]->exit_status == EXIT_SUCCESS ? "OK" : "ERR");
    printf("Provider exit status : %s\n",
           thr_rets[1]->exit_status == EXIT_SUCCESS ? "OK" : "ERR");

err_free:
    config->free_exec();
exit:
    return ret;
}

int exec_option(exec_option_t option, const char *if_name,
                packet_params_t packet_info)
{
    exec_option_config_t config;

    switch (option) {
    case OPT1:
        config.init_exec = init_exec_option1;
        config.free_exec = free_exec_option1;
        config.update_cb = sniffer_update_stat1;
        config.retrieve_cb = provider_retrieve_stat1;
        break;
    case OPT2:
        config.init_exec = init_exec_option2;
        config.free_exec = free_exec_option2;
        config.update_cb = sniffer_update_stat2;
        config.retrieve_cb = provider_retrieve_stat2;
        break;
    default:
        fprintf(stderr, "Wrong execution option\n");
        return EXIT_FAILURE;
    }

    return execute(&config, if_name, packet_info);
}

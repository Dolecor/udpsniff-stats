/*
 * Copyright (c) 2022 Dmitry Dolenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <net/if.h>
#include <string.h>

#include "common.h"
#include "printstats.h"

#if !USE_UBUS
#    include <libubus.h>
#    include <libubox/uloop.h>
#    include <libubox/blobmsg_json.h>
#    include <libubox/ustream.h>
#    include "ipc/ubus/ubus_binary_helper.h"
#    include "ipc/ubus/ubus_common.h"
#else
#    include "ipc/mq/mq_common.h"
#    include "ipc/mq/mq_interface_subscriber.h"
#endif

#define PROGRAM_NAME "print-stats"

#if !USE_UBUS
static struct ubus_context *ctx;
static struct blob_buf b;
static msg_reply_t reply;

static struct ubus_object sub_client_object = {};

static int get_stats(packet_params_t *params, statistics_t *stats, char *ifname)
{
    struct ubus_request req;
    uint32_t id;
    int ret;

    ret = ubus_add_object(ctx, &sub_client_object);
    if (ret) {
        fprintf(stderr, "Failed to add_object object: %s\n",
                ubus_strerror(ret));
        return;
    }

    if (ubus_lookup_id(ctx, "stats", &id)) {
        fprintf(stderr, "Failed to look up stats object\n");
        return;
    }

    blob_buf_init(&b, 0);
    // blobmsg_add_u32(&b, "id", sub_client_object.id);
    ubus_invoke(ctx, id, "get", b.head, (char *)&reply, 0, 3000);

    uloop_run();
}

static int execute()
{
    const char *ubus_socket = NULL;
    int ret = EXIT_SUCCESS;
    statistics_t stats;
    packet_params_t params;
    char ifname[IF_NAMESIZE];

    uloop_init();

    ctx = ubus_connect(ubus_socket);
    if (!ctx) {
        fprintf(stderr, "Failed to connect to ubus\n");
        return EXIT_FAILURE;
    }

    ubus_add_uloop(ctx);

    if (get_stats(&params, &stats, ifname)) {
        printparams(params, ifname);
        printstats(stats);
    } else {
        ret = EXIT_FAILURE;
    }

    ubus_free(ctx);
    uloop_done();

    return ret;
}
#else
static int execute()
{
    int ret = EXIT_SUCCESS;
    statistics_t stats;
    packet_params_t params;
    char ifname[IF_NAMESIZE];

    if (!init_mq_sub(MQ_SINGLE_PROV_NAME)) {
        return EXIT_FAILURE;
    }

    if (!get_stats(&params, &stats, ifname)) {
        free_mq_sub();
        return EXIT_FAILURE;
    }

    printparams(params, ifname);
    printstats(stats);

    free_mq_sub();
    return EXIT_SUCCESS;
}
#endif

int main(int argc, char *argv[])
{
    if (argc > 1) {
        fprintf(stderr, "Usage: %s\n", PROGRAM_NAME);
        exit(EXIT_FAILURE);
    }

    int ret = execute();
    exit(ret);
}
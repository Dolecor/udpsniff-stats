/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#include "udpsniff/mq_interface.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"
#include "mq_common.h"

#define MAX_PENDING_REQ 10

static uint8_t inited = 0;
static mqd_t mqd_provider; /* MQ to read requests for stats */
static msg_request_t last_req;

int init_mq(packet_params_t params, const char *ifname)
{
    if (inited) {
        return 0;
    }

    const int flags = (O_CREAT | O_EXCL | O_RDONLY | O_NONBLOCK);
    const mode_t perms = (S_IRUSR | S_IWUSR);
    struct mq_attr attr = {.mq_maxmsg = MAX_PENDING_REQ,
                           .mq_msgsize = sizeof(msg_request_t)};

    mqd_provider = mq_open(MQ_SINGLE_PROV_NAME, flags, perms, &attr);
    if (mqd_provider == (mqd_t)-1) {
        if (errno == EEXIST) {
            fprintf(stderr, "Instance of udp-sniff is already running.\n");
        } else {
            perror("mq_open(mq_provider_name)");
        }
        return 0;
    }

    inited = 1;
    return 1;
}

void free_mq()
{
    if (inited) {
        mq_close(mqd_provider);
        mq_unlink(MQ_SINGLE_PROV_NAME);
        inited = 0;
    }
}

int check_request()
{
    int ret =
        mq_receive(mqd_provider, (char *)&last_req, sizeof(last_req), NULL);

    struct mq_attr tmp_attr;
    mq_getattr(mqd_provider, &tmp_attr);

    if ((ret < 0) && (errno == EAGAIN)) { /* no pending requests */
        return 0;
    } else if (ret < 0) { /* something really bad */
        perror("mq_receive");
        free_mq();
        exit(EXIT_FAILURE);
    }

    return 1;
}

int send_reply(packet_params_t params, statistics_t stats, const char *ifname)
{
    mqd_t mqd_tmp; /* MQ to send reply */
    msg_reply_t msg_reply = {.params = params, .stats = stats};
    strncpy(msg_reply.ifname, ifname, IF_NAMESIZE);

    mqd_tmp = mq_open(last_req.mq_reply_name, O_WRONLY);
    if (mqd_tmp == (mqd_t)-1) {
        return 0;
    }

    if (mq_send(mqd_tmp, (const char *)&msg_reply, sizeof(msg_reply), 0)
        == -1) {
        mq_close(mqd_tmp);
        return 0;
    }

    mq_close(mqd_tmp);
    return 1;
}

/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#include "udpsniff/mq_interface.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <unistd.h>

#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"
#include "mq_common.h"

#define MAX_PENDING_REQ 1

static char mq_provider_name[MQ_PROV_NAME_SIZE];
static mqd_t mqd_provider; /* MQ to read requests for stats */
static msg_request_t last_req;

int init_mq(packet_params_t params)
{
    const int flags = (O_CREAT | O_EXCL | O_RDONLY | O_NONBLOCK);
    const mode_t perms = (S_IRUSR | S_IWUSR);
    struct mq_attr attr = {.mq_maxmsg = MAX_PENDING_REQ,
                                 .mq_msgsize = sizeof(msg_request_t)};

    generate_mq_prov_name(params, mq_provider_name);
    mqd_provider = mq_open(mq_provider_name, flags, perms, &attr);
    if (mqd_provider == (mqd_t)-1) {
        perror("mq_open(mq_reply_name)");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int free_mq()
{
    mq_close(mqd_provider);
    mq_unlink(mq_provider_name);
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

    printf("msgsize : %ld\n", tmp_attr.mq_msgsize);
    printf("structsize : %ld\n", sizeof(last_req));

    return 1;
}

int send_reply(statistics_t reply)
{
    mqd_t mqd_tmp; /* MQ to send reply */
    msg_reply_t msg_reply = {.stats = reply};

    mqd_tmp = mq_open(last_req.mq_reply_name, O_WRONLY);
    if (mqd_tmp == (mqd_t)-1) {
        return 0;
    }

    if (mq_send(mqd_tmp, (const char *)&msg_reply, sizeof(msg_reply), 0)) {
        mq_close(mqd_tmp);
        return 0;
    }

    mq_close(mqd_tmp);
    return 1;
}

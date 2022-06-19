/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#include "printstats/mq_interface.h"

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"
#include "mq_common.h"

static char mq_reply_name[MQ_REPLY_NAME_SIZE];
static mqd_t mqd_provider; /* MQ to send request to provider */
static mqd_t mqd_reply;    /* MQ that provider should use to send reply */

int init_mq(const char *mq_provider_name)
{
    const int flags = (O_CREAT | O_EXCL | O_RDONLY);
    const mode_t perms = (S_IRUSR | S_IWUSR);
    const struct mq_attr reply_attr = {.mq_maxmsg = 1,
                                       .mq_msgsize = sizeof(msg_reply_t)};

    mqd_provider = mq_open(mq_provider_name, O_WRONLY);
    if (mqd_provider == (mqd_t)-1) {
        perror("No such mq provider");
        return EXIT_FAILURE;
    }

    generate_mq_reply_name(getpid(), mq_reply_name);
    mqd_reply = mq_open(mq_reply_name, flags, perms, &reply_attr);
    if (mqd_reply == (mqd_t)-1) {
        perror("mq_open(mq_reply_name)");
        mq_close(mqd_provider);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int free_mq()
{
    mq_close(mqd_provider);
    mq_close(mqd_reply);
    mq_unlink(mq_reply_name);
}

int get_stats(statistics_t *reply)
{
    msg_request_t req;
    msg_reply_t rep;

    strcpy(req.mq_reply_name, mq_reply_name);
    if (mq_send(mqd_provider, (const char *)&req, sizeof(req), 0) == -1) {
        perror("mq_send");
        return EXIT_FAILURE;
    }

    if (mq_receive(mqd_reply, (char *)&rep, sizeof(rep), NULL) == -1) {
        perror("mq_receive");
        return EXIT_FAILURE;
    }

    *reply = rep.stats;

    return EXIT_SUCCESS;
}

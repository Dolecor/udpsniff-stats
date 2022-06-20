/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#include "udpsniff/exec_options/priv_exec_option1.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "udpsniff/exec_options/priv_exec_options.h"

static int pipe_fds[2];
static statistics_t sniffer_tmp;
static statistics_t provider_tmp = {.packets = 0, .bytes = 0};

int init_exec_option1()
{
    if (pipe(pipe_fds) == -1) {
        perror("pipe");
        return RCEXEC_GENERAL_ERR;
    }

    if (fcntl(pipe_fds[0], F_SETFL, O_NONBLOCK) == -1) {
        perror("fcntl");
        return RCEXEC_GENERAL_ERR;
    }

    return RCEXEC_OK;
}

void free_exec_option1()
{
    close(pipe_fds[0]);
    close(pipe_fds[1]);
}

int sniffer_update_stat1(size_t bytes)
{
    sniffer_tmp.packets = 1;
    sniffer_tmp.bytes = bytes;

    if ((write(pipe_fds[1], &sniffer_tmp, sizeof(sniffer_tmp)) == -1)
        && (errno != EAGAIN)) {
        perror("write");
        return RCEXEC_BROKEN_IPC;
    }

    return RCEXEC_OK;
}

int provider_retrieve_stat1(statistics_t *stat)
{
    ssize_t num_bytes;

    errno = 0;
    num_bytes = read(pipe_fds[0], &provider_tmp, sizeof(provider_tmp));
    if ((num_bytes == -1) && (errno != EAGAIN)) {
        perror("read");
        return RCEXEC_BROKEN_IPC;
    } else if ((num_bytes == 0) || (errno == EAGAIN)) {
        return RCEXEC_OK; /* no new data, do not update stat*/
    }

    stat->packets += 1;
    stat->bytes += provider_tmp.bytes;

    return RCEXEC_OK;
}

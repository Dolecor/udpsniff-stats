/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#include "priv_exec_option1.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int pipe_fds[2];
static statistics_t sniffer_tmp;
static statistics_t provider_tmp;

int init_exec_option1()
{
    if (pipe(pipe_fds) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
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

    if (write(pipe_fds[1], &sniffer_tmp, sizeof(sniffer_tmp)) < 0) {
        perror("write");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int provider_retrieve_stat1(statistics_t *stat)
{
    ssize_t num_bytes;

    num_bytes = read(pipe_fds[0], &provider_tmp, sizeof(provider_tmp));
    if (num_bytes < 0) {
        perror("read");
        return EXIT_FAILURE;
    } else if (num_bytes == 0) {
        return EXIT_SUCCESS; /* TODO: Maybe add more descriptive return codes to
                                exec_options interface? */
    }

    stat->packets += 1;
    stat->bytes += provider_tmp.bytes;

    return EXIT_SUCCESS;
}

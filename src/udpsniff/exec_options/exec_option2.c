/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#include "udpsniff/exec_options/priv_exec_option2.h"

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "udpsniff/exec_options/priv_exec_options.h"

static statistics_t glob_stat = {.packets = 0, .bytes = 0};
static statistics_t local_stat;
static pthread_mutex_t mtx_stat = PTHREAD_MUTEX_INITIALIZER;

int init_exec_option2()
{
    local_stat.packets = 0;
    local_stat.bytes = 0;

    return RCEXEC_OK;
}

void free_exec_option2()
{
    local_stat.packets = 0;
    local_stat.bytes = 0;
}

int sniffer_update_stat2(size_t bytes)
{
    local_stat.packets += 1;
    local_stat.bytes += bytes;

    if (pthread_mutex_lock(&mtx_stat) != 0) {
        return RCEXEC_BROKEN_IPC;
    }

    glob_stat = local_stat;

    if (pthread_mutex_unlock(&mtx_stat) != 0) {
        return RCEXEC_BROKEN_IPC;
    }

    return RCEXEC_OK;
}

int provider_retrieve_stat2(statistics_t *stat)
{
    if (pthread_mutex_lock(&mtx_stat) != 0) {
        return RCEXEC_BROKEN_IPC;
    }

    *stat = glob_stat;

    if (pthread_mutex_unlock(&mtx_stat) != 0) {
        return RCEXEC_BROKEN_IPC;
    }

    return RCEXEC_OK;
}

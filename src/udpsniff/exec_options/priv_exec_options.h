/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef PRIV_EXEC_OPTIONS_H
#define PRIV_EXEC_OPTIONS_H

#include <stddef.h>

#include "common.h"

/* Return codes */
typedef int rc_exec_t;
#define RCEXEC(code) ((rc_exec_t)(code))

#define RCEXEC_OK           RCEXEC(0)
#define RCEXEC_GENERAL_ERR  RCEXEC(1)
#define RCEXEC_BROKEN_IPC   RCEXEC(2)

typedef rc_exec_t (*sniffer_update_stat_cb)(size_t bytes);
typedef rc_exec_t (*provider_retrieve_stat_cb)(statistics_t *stat);

typedef struct exec_option_config {
    rc_exec_t (*init_exec)();
    void (*free_exec)();
    sniffer_update_stat_cb update_cb;
    provider_retrieve_stat_cb retrieve_cb;
} exec_option_config_t;

#endif /* PRIV_EXEC_OPTIONS_H */

/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef PRIV_EXEC_OPTION2_H
#define PRIV_EXEC_OPTION2_H

#include "udpsniff/exec_options/priv_exec_options.h"
#include "common.h"

rc_exec_t init_exec_option2();
void free_exec_option2();
rc_exec_t sniffer_update_stat2(size_t bytes);
rc_exec_t provider_retrieve_stat2(statistics_t *stat);

#endif /* PRIV_EXEC_OPTION2_H */

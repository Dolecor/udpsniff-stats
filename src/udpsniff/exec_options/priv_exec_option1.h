/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef PRIV_EXEC_OPTION1_H
#define PRIV_EXEC_OPTION1_H

#include "udpsniff/exec_options/priv_exec_options.h"
#include "common.h"

rc_exec_t init_exec_option1();
void free_exec_option1();
rc_exec_t sniffer_update_stat1(size_t bytes);
rc_exec_t provider_retrieve_stat1(statistics_t *stat);

#endif /* PRIV_EXEC_OPTION1_H */

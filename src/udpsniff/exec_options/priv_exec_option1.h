/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef PRIV_EXEC_OPTION1_H
#define PRIV_EXEC_OPTION1_H

#include "common.h"

int init_exec_option1();
void free_exec_option1();
int sniffer_update_stat1(size_t bytes);
int provider_retrieve_stat1(statistics_t *stat);

#endif /* PRIV_EXEC_OPTION1_H */

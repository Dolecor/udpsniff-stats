/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef PRIV_EXEC_OPTION2_H
#define PRIV_EXEC_OPTION2_H

#include "common.h"

int init_exec_option2();
void free_exec_option2();
int sniffer_update_stat2(size_t bytes);
int provider_retrieve_stat2(statistics_t *stat);

#endif /* PRIV_EXEC_OPTION2_H */

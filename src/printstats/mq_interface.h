/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef MQ_INTERFACE_H
#define MQ_INTERFACE_H

#include "common.h"

int init_mq(const char *mq_provider_name);
int free_mq();
int get_stats(statistics_t *reply);

#endif /* MQ_INTERFACE_H */

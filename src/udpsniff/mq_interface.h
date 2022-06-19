/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef MQ_INTERFACE_H
#define MQ_INTERFACE_H

#include "common.h"

int init_mq(packet_params_t params);
int free_mq();
int check_request();
int send_reply(statistics_t reply);

#endif /* MQ_INTERFACE_H */

/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef MQ_INTERFACE_H
#define MQ_INTERFACE_H

#include "common.h"

int init_mq_prov(packet_params_t params, const char *ifname);
void free_mq_prov();
int check_request();
int send_reply(packet_params_t params, statistics_t stats, const char *ifname);

#endif /* MQ_INTERFACE_H */

/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef UBUS_COMMON_H
#define UBUS_COMMON_H

#include <net/if.h>

#include "common.h"

typedef struct msg_reply {
    statistics_t stats;
    packet_params_t params;
    char ifname[IF_NAMESIZE];
} msg_reply_t;

#endif /* UBUS_COMMON_H */
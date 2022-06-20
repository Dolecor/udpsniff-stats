/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef MQ_COMMON_H
#define MQ_COMMON_H

#include "common.h"

#define sep "_"

#define MQ_IFNAME_MASK "xxxxxxxxxxxxxxx" /* IF_NAMESIZE symbols */
#define IP_MASK "xxx.xxx.xxx.xxx"
#define PORT_MASK "xxxxx"
#define MQ_NAME_PREFIX "mq-udpsniff"
#define MQ_PROV_NAME_MASK                                                \
    ("/" MQ_NAME_PREFIX sep MQ_IFNAME_MASK sep IP_MASK sep PORT_MASK sep \
         IP_MASK sep PORT_MASK)
#define MQ_PROV_NAME_SIZE sizeof(MQ_PROV_NAME_MASK)
#define MQ_PROV_NAME_FMTSTR ("/%s" sep "%s" sep "%s" sep "%s" sep "%s" sep "%s")

#define MQ_REPLY_PREFIX "mq-udpsniff-reply"
#define MQ_REPLY_POSTFIX "xxxxxxx" /* print-stats' PID */
#define MQ_REPLY_NAME_MASK ("/" MQ_REPLY_PREFIX sep MQ_REPLY_POSTFIX)
#define MQ_REPLY_NAME_SIZE sizeof(MQ_REPLY_NAME_MASK)
#define MQ_REPLY_NAME_FMTSTR ("/%s" sep "%d")

typedef struct msg_request {
    char mq_reply_name[MQ_REPLY_NAME_SIZE];
} msg_request_t;

typedef struct msg_reply {
    statistics_t stats;
} msg_reply_t;

void generate_mq_prov_name(packet_params_t params, const char *ifname,
                           char *buf);
void decode_params(packet_params_t *params, char *ifname, const char *buf);

void generate_mq_reply_name(pid_t postfix, char *buf);

#endif /* MQ_COMMON_H */

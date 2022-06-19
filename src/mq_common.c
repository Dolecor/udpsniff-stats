/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#include "mq_common.h"

#include <string.h>
#include <stdio.h>
#include <netinet/in.h>

#include "netinet_helper.h"

void generate_mq_prov_name(packet_params_t params, char *buf)
{
    char src_ip[INET_ADDRSTRLEN];
    char src_port[PORTSTRLEN];
    char dest_ip[INET_ADDRSTRLEN];
    char dest_port[PORTSTRLEN];

    inaddr_to_str(params.src_ip, src_ip);
    port_to_str(params.src_port, src_port);
    inaddr_to_str(params.dest_ip, dest_ip);
    port_to_str(params.dest_port, dest_port);

    sprintf(buf, MQ_PROV_NAME_FMTSTR,
            MQ_NAME_PREFIX, src_ip, src_port, dest_ip, dest_port);
}

void decode_params(packet_params_t *params, const char *buf)
{
    const char delim[] = sep;
    char *token;
    char str[MQ_PROV_NAME_SIZE];
    char src_ip[INET_ADDRSTRLEN];
    char src_port[PORTSTRLEN];
    char dest_ip[INET_ADDRSTRLEN];
    char dest_port[PORTSTRLEN];

    strcpy(str, buf);

    token = strtok(str, delim);  /* mq name prefix, skip */
    token = strtok(NULL, delim); /* source ip */
    strcpy(src_ip, token);
    token = strtok(NULL, delim); /* source port */
    strcpy(src_port, token);
    token = strtok(NULL, delim); /* destination ip */
    strcpy(dest_ip, token);
    token = strtok(NULL, delim); /* destination port */
    strcpy(dest_port, token);

    str_to_inaddr(&params->src_ip, src_ip);
    str_to_port(&params->src_port, src_port);
    str_to_inaddr(&params->dest_ip, dest_ip);
    str_to_port(&params->dest_port, dest_port);
}

void generate_mq_reply_name(pid_t postfix, char *buf)
{
    sprintf(buf, MQ_REPLY_NAME_FMTSTR, MQ_REPLY_PREFIX, postfix);
}

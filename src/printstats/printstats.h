/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef PRINTSTATS_H
#define PRINTSTATS_H

#include "common.h"

void printparams(packet_params_t params, const char *ifname);
void printstats(statistics_t stats);
void printstats_cr(statistics_t stats);

#endif /* PRINTSTATS_H */

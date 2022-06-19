/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef EXEC_OPTIONS_H
#define EXEC_OPTIONS_H

#include "common.h"

typedef enum {
    OPT1,
    OPT2,
} exec_option_t;

int exec_option(exec_option_t option, const char *if_name,
                packet_params_t packet_params);

#endif /* EXEC_OPTIONS_H */

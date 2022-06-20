/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#include "control.h"

volatile sig_atomic_t stop_flag = 0;

void stop_handler(int sig)
{
    stop_flag = 1;
}

int set_signals()
{
    if ((signal(SIGINT, stop_handler) == SIG_ERR)
        || (signal(SIGQUIT, stop_handler) == SIG_ERR)
        || (signal(SIGTERM, stop_handler) == SIG_ERR)) {
        return 0;
    }

    return 1;
}

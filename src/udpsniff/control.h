/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef CONTROL_H
#define CONTROL_H

#include <signal.h>

extern volatile sig_atomic_t stop_flag;

void stop_handler(int sig);

int set_signals();

#endif /* CONTROL_H */

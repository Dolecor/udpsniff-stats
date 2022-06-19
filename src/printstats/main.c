/*
 * Copyright (c) 2022 Dmitry Dolenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"
#include "netinet_helper.h"
#include "printstats.h"
#include "mq_common.h"
#include "printstats/mq_interface.h"

#define PROGRAM_NAME "print-stats"

static void print_help_and_exit(char *msg)
{
}

static void parse_options(int argc, char *argv[])
{
}

int main(int argc, char *argv[])
{
    int ret = EXIT_SUCCESS;
    statistics_t reply;
    packet_params_t params;

    if (init_mq(argv[1])) {
        ret = EXIT_FAILURE;
        printf("%s\n", argv[1]);
        goto exit;
    }

    if (get_stats(&reply)) {
        ret = EXIT_FAILURE;
        goto err_free;
    }

    decode_params(&params, argv[1]);
    printparams(params);
    printstats(reply);

err_free:
    free_mq();
exit:
    exit(ret);
}
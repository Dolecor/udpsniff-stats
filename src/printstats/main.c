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
#include <net/if.h>
#include <string.h>
#include <getopt.h>

#include "common.h"
#include "printstats.h"
#include "mq_common.h"
#include "printstats/mq_interface.h"

#define PROGRAM_NAME "print-stats"

static void print_help_and_exit(char *msg)
{
    if (msg != NULL) {
        fprintf(stderr, "%s\n\n", msg);
    }
    fprintf(stderr, "Usage: %s [-h|--help] <mq name>\n", PROGRAM_NAME);
    fprintf(stderr, "\n");
    fprintf(stderr, "  <mq name>     Message queue name (see 'ls /dev/mqueue'\n"
                    "                for available options).\n");
    fprintf(stderr, "  -h, --help    Print this help and exit.\n");

    exit(EXIT_FAILURE);
}

static void parse_options(int argc, char *argv[], char *mq_name)
{
    int opt;

    static const struct option long_opts[] = {
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}};

    while ((opt = getopt_long(argc, argv, "h", long_opts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            print_help_and_exit(NULL);
        default:
            print_help_and_exit(NULL);
        }
    }

    if (optind < argc) {
        strncpy(mq_name, argv[optind], MQ_PROV_NAME_SIZE);
    } else {
        print_help_and_exit("Message queue name must be specified.");
    }
}

static int execute(const char *mq_prov_name)
{
    int ret = EXIT_SUCCESS;
    statistics_t reply;
    packet_params_t params;
    char ifname[IF_NAMESIZE];

    if (!init_mq(mq_prov_name)) {
        ret = EXIT_FAILURE;
        goto exit;
    }

    if (!get_stats(&reply)) {
        ret = EXIT_FAILURE;
        goto err_free;
    }

    decode_params(&params, ifname, mq_prov_name);
    printparams(params, ifname);
    printstats(reply);

err_free:
    free_mq();
exit:
    return ret;
}

int main(int argc, char *argv[])
{
    int ret = EXIT_SUCCESS;
    char mq_provider_name[MQ_PROV_NAME_SIZE];

    parse_options(argc, argv, mq_provider_name);

    ret = execute(mq_provider_name);
    exit(ret);
}
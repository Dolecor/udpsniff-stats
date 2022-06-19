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
#include <getopt.h>
#include <limits.h>
#include <errno.h>

#include <net/if.h>
#include <arpa/inet.h>

#include "netinet_helper.h"
#include "exec_options/exec_options.h"

#define PROGRAM_NAME "udp-sniff"

typedef struct options {
    packet_params_t params;
    char if_name[IF_NAMESIZE];
    exec_option_t exec_opt;
} options_t;

#define DEFAULT_PACKET_PARAMS  \
    {                          \
        .src_ip = ANY_IP,      \
        .dest_ip = ANY_IP,     \
        .src_port = ANY_PORT,  \
        .dest_port = ANY_PORT, \
    }

static int ip_check_and_set(const char *ip_str, in_addr_t *ip);
static int port_check_and_set(const char *port_str, in_port_t *port);

static void print_help_and_exit(char *msg)
{
    if (msg != NULL) {
        fprintf(stderr, "%s\n\n", msg);
    }
    fprintf(stderr, "Usage: %s [options] <interface name>\n", PROGRAM_NAME);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  <interface name>        The name of the interface. (i.e eth0)\n");
    fprintf(stderr, "  -h, --help              Print this help and exit\n");
    fprintf(stderr, "  -e {opt1|opt2}          Select execution option.\n"
                    "                          Default value is opt2.\n");
    fprintf(stderr, "      --src-ip=<ip>       Specify the source ip address.\n");
    fprintf(stderr, "      --dest-ip=<ip>      Specify the destination ip address.\n");
    fprintf(stderr, "      --src-port=<port>   Specify the source port number.\n");
    fprintf(stderr, "      --dest-port=<port>  Specify the destination port number.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  If src-ip, dest-ip, src-port or dest-port is not specified,\n"
                    "  then corresponding option is set to the default value. This\n"
                    "  value means that any ip or port of the packet's parameters\n"
                    "  will be taken into account in the statistics.\n");

    exit(EXIT_FAILURE);
}

static void parse_options(int argc, char *argv[], options_t *options)
{
    int opt;

    enum {
        OPT_SRC_IP = CHAR_MAX + 1,
        OPT_DEST_IP,
        OPT_SRC_PORT,
        OPT_DEST_PORT,
    };

    static const struct option long_opts[] = {
        {"help", no_argument, NULL, 'h'},
        {"src-ip", required_argument, NULL, OPT_SRC_IP},
        {"dest-ip", required_argument, NULL, OPT_DEST_IP},
        {"src-port", required_argument, NULL, OPT_SRC_PORT},
        {"dest-port", required_argument, NULL, OPT_DEST_PORT},
        {NULL, 0, NULL, 0}};

    while ((opt = getopt_long(argc, argv, "e:h", long_opts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            print_help_and_exit(NULL);
        case 'e':
            if (!strncmp(optarg, "opt1", sizeof(optarg))) {
                options->exec_opt = OPT1;
            } else if (!strncmp(optarg, "opt2", sizeof(optarg))) {
                options->exec_opt = OPT2;
            } else {
                print_help_and_exit("Wrong execution option (-e).");
            }
            break;
        case OPT_SRC_IP:
            if (!ip_check_and_set(optarg, &options->params.src_ip)) {
                fprintf(stderr, "%s is not IP address\n", optarg);
                exit(EXIT_FAILURE);
            }
            break;
        case OPT_DEST_IP:
            if (!ip_check_and_set(optarg, &options->params.dest_ip)) {
                fprintf(stderr, "%s is not IP address\n", optarg);
                exit(EXIT_FAILURE);
            }
            break;
        case OPT_SRC_PORT:
            if (!port_check_and_set(optarg, &options->params.src_port)) {
                fprintf(stderr, "%s is not valid port number\n", optarg);
                exit(EXIT_FAILURE);
            }
            break;
        case OPT_DEST_PORT:
            if (!port_check_and_set(optarg, &options->params.dest_port)) {
                fprintf(stderr, "%s is not valid port number\n", optarg);
                exit(EXIT_FAILURE);
            }
            break;
        default:
            print_help_and_exit(NULL);
        }
    }

    if (optind < argc) {
        strncpy(options->if_name, argv[optind], IF_NAMESIZE);
    } else {
        print_help_and_exit("Interface name must be specified.");
    }
}

static int ip_check_and_set(const char *ip_str, in_addr_t *ip)
{
    in_addr_t tmp = inet_addr(ip_str);

    if (tmp == INADDR_NONE) {
        return 0;
    }

    *ip = tmp;

    return 1;
}

static int port_check_and_set(const char *port_str, in_port_t *port)
{
    char *endptr;
    uint32_t tmp = (uint32_t)strtoll(port_str, &endptr, 0);

    if ((errno != 0) || (endptr == port_str) || (tmp < 1) || (tmp > MAX_PORT)) {
        return 0;
    }

    *port = htons(tmp);

    return 1;
}

int main(int argc, char *argv[])
{
    int ret;
    options_t options = {.exec_opt = OPT2, .params = DEFAULT_PACKET_PARAMS};

    parse_options(argc, argv, &options);
    ret = exec_option(options.exec_opt, options.if_name, options.params);

    exit(ret);
}

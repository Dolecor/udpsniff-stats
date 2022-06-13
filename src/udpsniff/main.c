#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "exec_options/exec_options.h"

#define PROGRAM_NAME "udpsniff"

int main(void)
{
    int ret;
    packet_params_t params;
    exec_option_t opt = OPT2;

    const char if_name[IF_NAMESIZE] = "lo";
    const char src_ip[INET_ADDRSTRLEN] = "127.0.0.1";
    const char dest_ip[INET_ADDRSTRLEN] = "127.0.0.1";
    const char src_port[PORTSTRLEN] = "0"; // ANY_PORT
    const char dest_port[PORTSTRLEN] = "1234";

    params.src_ip = inet_addr(src_ip);
    params.dest_ip = inet_addr(dest_ip);
    params.src_port = htons(atoi(src_port));
    params.dest_port = htons(atoi(dest_port));

    ret = exec_option(opt, if_name, params);

    return ret;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "exec_option1.h"
#include "exec_option2.h"

#define PROGRAM_NAME "udpsniff"

int main(void)
{
    int ret;

    int (*exec_opt)(void);
    exec_opt = exec_option1;
    ret = exec_opt();

    return ret;
}

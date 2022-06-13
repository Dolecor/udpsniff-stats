#ifndef EXEC_OPTIONS_H
#define EXEC_OPTIONS_H

#include "common.h"

typedef enum {
    OPT1,
    OPT2,
} exec_option_t;

int exec_option(exec_option_t option, const char *if_name,
                   packet_params_t packet_info);

#endif /* EXEC_OPTIONS_H */

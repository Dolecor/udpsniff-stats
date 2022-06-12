#include "exec_options.h"

extern int exec_option1(const char *if_name, packet_params_t packet_info);
extern int exec_option2(const char *if_name, packet_params_t packet_info);

int run_option(exec_option_t option, const char *if_name,
               packet_params_t packet_info)
{
    int (*exec_opt)(const char *, packet_params_t);
    int ret = EXIT_SUCCESS;

    switch (option) {
    case OPT1:
        exec_opt = exec_option1;
        break;
    case OPT2:
        exec_opt = exec_option2;
        break;
    default:
        return EXIT_FAILURE;
    }

    ret = exec_opt(if_name, packet_info);

    return ret;
}

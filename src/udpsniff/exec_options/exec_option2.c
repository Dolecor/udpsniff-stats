#include "priv_exec_option2.h"

static statistics_t glob_stat = {.packets = 0, .bytes = 0};
static statistics_t local_stat;
static pthread_mutex_t mtx_stat = PTHREAD_MUTEX_INITIALIZER;

int init_exec_option2()
{
    local_stat.packets = 0;
    local_stat.bytes = 0;
    return EXIT_SUCCESS;
}

void free_exec_option2()
{
    return;
}

int sniffer_update_stat2(size_t bytes)
{
    local_stat.packets += 1;
    local_stat.bytes += bytes;

    if (pthread_mutex_lock(&mtx_stat) != 0) {
        return EXIT_FAILURE;
    }

    glob_stat = local_stat;

    if (pthread_mutex_unlock(&mtx_stat) != 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int provider_retrieve_stat2(statistics_t *stat)
{
    if (pthread_mutex_lock(&mtx_stat) != 0) {
        return EXIT_FAILURE;
    }

    *stat = glob_stat;

    if (pthread_mutex_unlock(&mtx_stat) != 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

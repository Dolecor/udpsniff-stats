#ifndef PRIV_EXEC_OPTION2_H
#define PRIV_EXEC_OPTION2_H

#include "common.h"

int init_exec_option2();
void free_exec_option2();
int sniffer_update_stat2(size_t bytes);
int provider_retrieve_stat2(statistics_t *stat);

#endif /* PRIV_EXEC_OPTION2_H */
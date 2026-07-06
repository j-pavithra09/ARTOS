#ifndef DEADLINE_H
#define DEADLINE_H

#include <stdint.h>

void     deadline_check_all(void);
uint32_t deadline_get_misses(int task_index);

#endif
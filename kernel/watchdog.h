#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <stdint.h>

void watchdog_init(uint32_t timeout_ms);
void watchdog_kick(int task_index);
void watchdog_check(void);

#endif
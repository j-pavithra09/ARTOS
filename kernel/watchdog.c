#include "watchdog.h"
#include "../hal/uart.h"
#include "kernel.h"

static uint32_t last_kick[MAX_TASKS];
static uint32_t watchdog_timeout = 50;

void watchdog_init(uint32_t timeout_ms) {
    watchdog_timeout = timeout_ms;
    int count = kernel_get_task_count();
    for (int i = 0; i < count; i++) {
        last_kick[i] = 0;
    }
    uart_puts("[WATCHDOG] Initialised\n");
}

void watchdog_kick(int task_index) {
    last_kick[task_index] = kernel_get_ticks();
}

void watchdog_check(void) {
    uint32_t tick  = kernel_get_ticks();
    int      count = kernel_get_task_count();

    for (int i = 0; i < count; i++) {
        TCB *t = kernel_get_task(i);
        if (t->state == TASK_SUSPENDED) continue;
        if (tick - last_kick[i] > watchdog_timeout) {
            uart_puts("[WATCHDOG] Task stalled: ");
            uart_puts(t->name);
            uart_puts("\n");
            if (t->asil == ASIL_D) {
                uart_puts("[WATCHDOG] CRITICAL — ASIL-D task stalled\n");
            }
        }
    }
}
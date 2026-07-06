#include "deadline.h"
#include "../hal/uart.h"
#include "kernel.h"

void deadline_check_all(void) {
    uint32_t tick = kernel_get_ticks();
    int      count = kernel_get_task_count();

    for (int i = 0; i < count; i++) {
        TCB *t = kernel_get_task(i);
        if (t->deadline_misses > 0) {
            uart_puts("[DEADLINE] ");
            uart_puts(t->name);
            uart_puts(" misses=");
            uart_put_uint(t->deadline_misses);
            uart_puts(" runs=");
            uart_put_uint(t->run_count);
            uart_puts("\n");
        }
    }
}

uint32_t deadline_get_misses(int task_index) {
    return kernel_get_task(task_index)->deadline_misses;
}
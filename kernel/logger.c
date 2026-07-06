#include "logger.h"
#include "../hal/uart.h"
#include "kernel.h"

static uint32_t log_count = 0;

void logger_init(void) {
    uart_puts("[LOGGER] Started\n");
    uart_puts("tick,task,algo,misses,mode\n");
}

void logger_log_tick(void) {
    log_count++;
    if (log_count % 100 != 0) return;

    uint32_t tick  = kernel_get_ticks();
    int      count = kernel_get_task_count();

    for (int i = 0; i < count; i++) {
        TCB *t = kernel_get_task(i);
        uart_put_uint(tick);
        uart_puts(",");
        uart_puts(t->name);
        uart_puts(",");
        uart_put_uint(t->run_count);
        uart_puts(",");
        uart_put_uint(t->deadline_misses);
        uart_puts(",");
        uart_puts(kernel_get_mode() == 0 ? "NORMAL" : "OVERLOAD");
        uart_puts("\n");
    }
}

void logger_print_summary(void) {
    uart_puts("\n[SUMMARY] Final Results\n");
    uart_puts("========================\n");
    int count = kernel_get_task_count();
    for (int i = 0; i < count; i++) {
        TCB *t = kernel_get_task(i);
        uart_puts(t->name);
        uart_puts(" | runs=");
        uart_put_uint(t->run_count);
        uart_puts(" | misses=");
        uart_put_uint(t->deadline_misses);
        uart_puts("\n");
    }
}
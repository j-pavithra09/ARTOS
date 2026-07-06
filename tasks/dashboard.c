#include "adas_tasks.h"
#include "../hal/uart.h"
#include "../kernel/kernel.h"
#include "../kernel/deadline.h"
#include "../kernel/logger.h"
#include "../kernel/watchdog.h"

static uint32_t exec_count = 0;

void dashboard_task(void) {
    exec_count++;
    watchdog_kick(3);

    uart_puts("\n[DASH] t=");
    uart_put_uint(kernel_get_ticks());
    uart_puts("ms | Mode: ");
    uart_puts(kernel_get_mode() == 0 ? "NORMAL" : "OVERLOAD");
    uart_puts("\n");

    deadline_check_all();
    logger_log_tick();
    watchdog_check();
}
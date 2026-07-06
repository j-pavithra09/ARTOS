#include "hal/uart.h"
#include "kernel/kernel.h"
#include "kernel/logger.h"
#include "kernel/watchdog.h"
#include "tasks/adas_tasks.h"

int main(void) {

    uart_init();

    uart_puts("\n");
    uart_puts("╔══════════════════════════════════════╗\n");
    uart_puts("║   ADAS RTOS — Bare Metal ARM Cortex  ║\n");
    uart_puts("║   QEMU lm3s6965evb — No Linux        ║\n");
    uart_puts("╚══════════════════════════════════════╝\n\n");

    /*
     * Change algorithm here to test different schedulers:
     * ROUND_ROBIN
     * FIXED_PRIORITY
     * RATE_MONOTONIC
     * EARLIEST_DEADLINE
     * MIXED_CRITICALITY
     */
    kernel_init(MIXED_CRITICALITY);

    /* Create ADAS tasks
     * Args: function, name, priority, ASIL, period_ms, deadline_ms */
    kernel_task_create(braking_task,    "Braking",   7, ASIL_D,  10,  8);
    kernel_task_create(radar_task,      "Radar",     5, ASIL_B,  20, 18);
    kernel_task_create(camera_task,     "Camera",    3, ASIL_A,  33, 30);
    kernel_task_create(dashboard_task,  "Dashboard", 1, ASIL_QM,100, 95);

    logger_init();
    watchdog_init(50);

    kernel_start();

    return 0;
}
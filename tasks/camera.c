#include "adas_tasks.h"
#include "../hal/uart.h"
#include "../kernel/kernel.h"
#include "../kernel/watchdog.h"

static uint32_t exec_count   = 0;
static uint32_t lane_offset  = 0;

void camera_task(void) {
    exec_count++;
    watchdog_kick(2);

    uint32_t tick = kernel_get_ticks();
    lane_offset = (tick / 33) % 20;

    if (exec_count % 15 == 0) {
        uart_puts("[CAMERA] t=");
        uart_put_uint(tick);
        uart_puts("ms lane=");
        uart_put_uint(lane_offset);
        uart_puts("cm");
        if (lane_offset > 15) uart_puts(" [LANE DEPARTURE]");
        uart_puts("\n");
    }
}
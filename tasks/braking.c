#include "adas_tasks.h"
#include "../hal/uart.h"
#include "../kernel/kernel.h"
#include "../kernel/watchdog.h"
#include "../kitti_trace.h"

static uint32_t exec_count = 0;
static uint32_t frame      = 0;

void braking_task(void) {
    exec_count++;
    watchdog_kick(0);

    uint32_t distance = kitti_radar_cm[frame % KITTI_FRAMES];
    frame++;

    uint32_t pressure = 0;
    if      (distance < 50)  pressure = 100;
    else if (distance < 100) pressure = 50;
    else                     pressure = 0;

    if (exec_count % 50 == 0) {
        uart_puts("[BRAKE] t=");
        uart_put_uint(kernel_get_ticks());
        uart_puts("ms dist=");
        uart_put_uint(distance);
        uart_puts("cm pressure=");
        uart_put_uint(pressure);
        uart_puts("%\n");
    }
}
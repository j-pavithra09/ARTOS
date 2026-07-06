#include "adas_tasks.h"
#include "../hal/uart.h"
#include "../kernel/kernel.h"
#include "../kernel/watchdog.h"
#include "../kitti_trace.h"

static uint32_t exec_count = 0;
static uint32_t frame      = 0;

void radar_task(void) {
    exec_count++;
    watchdog_kick(1);

    uint32_t distance = kitti_radar_cm[frame % KITTI_FRAMES];
    frame++;

    if (exec_count % 25 == 0) {
        uart_puts("[RADAR] t=");
        uart_put_uint(kernel_get_ticks());
        uart_puts("ms dist=");
        uart_put_uint(distance);
        uart_puts("cm");
        if (distance < 50)  uart_puts(" [EMERGENCY]");
        else if (distance < 100) uart_puts(" [WARNING]");
        uart_puts("\n");
    }
}
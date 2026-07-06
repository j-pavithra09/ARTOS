#include "systick.h"
#include "../kernel/kernel.h"

/* SysTick registers — same address on ALL ARM Cortex-M chips */
#define SYSTICK_CTRL    (*(volatile uint32_t *)0xE000E010)
#define SYSTICK_LOAD    (*(volatile uint32_t *)0xE000E014)
#define SYSTICK_VAL     (*(volatile uint32_t *)0xE000E018)

#define SYSTICK_CTRL_ENABLE    (1 << 0)
#define SYSTICK_CTRL_TICKINT   (1 << 1)
#define SYSTICK_CTRL_CLKSOURCE (1 << 2)

#define CPU_FREQ_HZ     12000000UL
#define SYSTICK_HZ      1000

void systick_init(void) {
    SYSTICK_LOAD = (CPU_FREQ_HZ / SYSTICK_HZ) - 1;
    SYSTICK_VAL  = 0;
    SYSTICK_CTRL = SYSTICK_CTRL_ENABLE    |
                   SYSTICK_CTRL_TICKINT   |
                   SYSTICK_CTRL_CLKSOURCE;
}

/* This fires every 1ms — overrides weak default in startup.s */
void SysTick_Handler(void) {
    kernel_tick();
}
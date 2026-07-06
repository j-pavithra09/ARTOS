#ifndef UART_H
#define UART_H

#include <stdint.h>

/* UART0 hardware registers — lm3s6965 specific addresses */
#define UART0_DR        (*(volatile uint32_t *)0x4000C000)
#define UART0_FR        (*(volatile uint32_t *)0x4000C018)
#define UART0_FR_TXFF   (1 << 5)
#define SYSCTL_RCGC1    (*(volatile uint32_t *)0x400FE104)
#define SYSCTL_RCGC1_UART0 (1 << 0)

static inline void uart_init(void) {
    SYSCTL_RCGC1 |= SYSCTL_RCGC1_UART0;
    volatile int i;
    for (i = 0; i < 100; i++) { __asm__("nop"); }
}

static inline void uart_putc(char c) {
    while (UART0_FR & UART0_FR_TXFF) { }
    UART0_DR = (uint32_t)c;
}

static inline void uart_puts(const char *s) {
    while (*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
}

static inline void uart_put_uint(uint32_t n) {
    if (n == 0) { uart_putc('0'); return; }
    char buf[10];
    int i = 0;
    while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    while (i > 0) uart_putc(buf[--i]);
}

static inline void uart_put_int(int32_t n) {
    if (n < 0) { uart_putc('-'); n = -n; }
    uart_put_uint((uint32_t)n);
}

#endif
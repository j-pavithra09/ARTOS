.syntax unified
    .cpu cortex-m3
    .thumb

    .section .isr_vector, "a"
    .align 2
    .global _vectors

_vectors:
    .word   _stack_top
    .word   Reset_Handler
    .word   Default_Handler
    .word   Default_Handler
    .word   Default_Handler
    .word   Default_Handler
    .word   Default_Handler
    .word   0
    .word   0
    .word   0
    .word   0
    .word   Default_Handler
    .word   Default_Handler
    .word   0
    .word   PendSV_Handler
    .word   SysTick_Handler

    .section .text
    .thumb_func
    .global Reset_Handler
Reset_Handler:
    ldr     r0, =_data_start
    ldr     r1, =_data_load
    ldr     r2, =_data_end
    b       copy_check
copy_loop:
    ldr     r3, [r1], #4
    str     r3, [r0], #4
copy_check:
    cmp     r0, r2
    bcc     copy_loop

    ldr     r0, =_bss_start
    ldr     r1, =_bss_end
    mov     r2, #0
    b       bss_check
bss_loop:
    str     r2, [r0], #4
bss_check:
    cmp     r0, r1
    bcc     bss_loop

    bl      main
hang:
    b       hang

    .thumb_func
    .global PendSV_Handler
    .weak   PendSV_Handler
PendSV_Handler:
    bx      lr

    .thumb_func
    .global SysTick_Handler
    .weak   SysTick_Handler
SysTick_Handler:
    bx      lr

    .thumb_func
    .global Default_Handler
Default_Handler:
    b       Default_Handler
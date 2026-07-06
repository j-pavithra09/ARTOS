.syntax unified
    .cpu cortex-m3
    .thumb

/*
 * PendSV_Handler — Context Switch
 *
 * Called by ARM hardware when PendSV interrupt fires.
 * Saves current task registers, loads next task registers.
 *
 * ARM auto-saves R0-R3, R12, LR, PC, xPSR on interrupt entry.
 * We manually save R4-R11 — the callee-saved registers.
 */

    .global PendSV_Handler
    .thumb_func
PendSV_Handler:
    /* Disable interrupts during context switch */
    cpsid   i

    /* Get current PSP (task stack pointer) */
    mrs     r0, psp
    isb

    /* Save R4-R11 onto current task stack */
    stmdb   r0!, {r4-r11}

    /* Save updated PSP into current TCB->stack_ptr */
    ldr     r1, =current_tcb
    ldr     r1, [r1]
    str     r0, [r1]

    /* Load next TCB->stack_ptr */
    ldr     r1, =next_tcb
    ldr     r1, [r1]
    str     r1, [r1]
    ldr     r0, [r1]

    /* Restore R4-R11 from next task stack */
    ldmia   r0!, {r4-r11}

    /* Update PSP to next task stack */
    msr     psp, r0
    isb

    /* Re-enable interrupts */
    cpsie   i

    /* Return to thread mode using PSP */
    ldr     lr, =0xFFFFFFFD
    bx      lr

    .global current_tcb
    .global next_tcb
current_tcb:
    .word   0
next_tcb:
    .word   0
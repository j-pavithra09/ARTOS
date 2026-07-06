#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

/* ── Configuration ────────────────────────────────── */
#define MAX_TASKS       8
#define STACK_SIZE      256
#define SYSTICK_HZ      1000
#define CPU_FREQ_HZ     12000000UL

/* ── Scheduling Algorithms ────────────────────────── */
typedef enum {
    ROUND_ROBIN        = 0,
    FIXED_PRIORITY     = 1,
    RATE_MONOTONIC     = 2,
    EARLIEST_DEADLINE  = 3,
    MIXED_CRITICALITY  = 4
} SchedAlgo;

/* ── ASIL Safety Levels ───────────────────────────── */
typedef enum {
    ASIL_QM = 0,
    ASIL_A  = 1,
    ASIL_B  = 2,
    ASIL_D  = 3
} AsilLevel;

/* ── Task States ──────────────────────────────────── */
typedef enum {
    TASK_READY     = 0,
    TASK_RUNNING   = 1,
    TASK_BLOCKED   = 2,
    TASK_SUSPENDED = 3,
    TASK_DEAD      = 4
} TaskState;

/* ── Task Control Block ───────────────────────────── */
typedef struct {
    uint32_t    *stack_ptr;          /* MUST BE FIRST */
    uint32_t     stack[STACK_SIZE];  /* private stack  */
    const char  *name;
    uint8_t      priority;
    TaskState    state;
    AsilLevel    asil;
    uint32_t     period_ms;
    uint32_t     deadline_ms;
    uint32_t     base_period_ms;
    uint32_t     next_run;
    uint32_t     deadline_misses;
    uint32_t     run_count;
    uint32_t     last_start;
    void       (*function)(void);
} TCB;

/* ── System Mode ──────────────────────────────────── */
typedef enum {
    MODE_NORMAL   = 0,
    MODE_OVERLOAD = 1
} SystemMode;

/* ── Kernel API ───────────────────────────────────── */
void     kernel_init(SchedAlgo algo);
void     kernel_task_create(void (*fn)(void), const char *name,
                             uint8_t priority, AsilLevel asil,
                             uint32_t period_ms, uint32_t deadline_ms);
void     kernel_start(void);
void     kernel_tick(void);
void     task_yield(void);
uint32_t kernel_get_ticks(void);
TCB*     kernel_get_task(int index);
int      kernel_get_task_count(void);
SystemMode kernel_get_mode(void);

#endif
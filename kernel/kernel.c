#include "kernel.h"
#include "../hal/uart.h"
#include <stddef.h>

/* ── Static Task Pool ─────────────────────────────── */
static TCB        task_pool[MAX_TASKS];
static int        task_count   = 0;
static int        current_task = -1;
static uint32_t   tick_count   = 0;
static SchedAlgo  sched_algo   = MIXED_CRITICALITY;
static SystemMode sys_mode     = MODE_NORMAL;
static int        rr_index     = 0;

/* ── kernel_init ──────────────────────────────────── */
void kernel_init(SchedAlgo algo) {
    sched_algo = algo;
    task_count = 0;
    tick_count = 0;
    current_task = -1;
    sys_mode = MODE_NORMAL;

    uart_puts("[KERNEL] Initialised | Algo: ");
    switch(algo) {
        case ROUND_ROBIN:       uart_puts("Round Robin\n");       break;
        case FIXED_PRIORITY:    uart_puts("Fixed Priority\n");    break;
        case RATE_MONOTONIC:    uart_puts("Rate Monotonic\n");    break;
        case EARLIEST_DEADLINE: uart_puts("EDF\n");               break;
        case MIXED_CRITICALITY: uart_puts("Mixed Criticality\n"); break;
    }
}

/* ── kernel_task_create ───────────────────────────── */
void kernel_task_create(void (*fn)(void), const char *name,
                         uint8_t priority, AsilLevel asil,
                         uint32_t period_ms, uint32_t deadline_ms) {
    if (task_count >= MAX_TASKS) return;

    TCB *t           = &task_pool[task_count];
    t->function      = fn;
    t->name          = name;
    t->priority      = priority;
    t->asil          = asil;
    t->period_ms     = period_ms;
    t->base_period_ms= period_ms;
    t->deadline_ms   = deadline_ms;
    t->state         = TASK_READY;
    t->next_run      = 0;
    t->deadline_misses = 0;
    t->run_count     = 0;
    t->last_start    = 0;
    t->stack_ptr     = &t->stack[STACK_SIZE - 1];

    task_count++;

    uart_puts("[KERNEL] Task created: ");
    uart_puts(name);
    uart_puts("\n");
}

/* ── Overload Detection ───────────────────────────── */
static void check_overload(void) {
    uint32_t ready_count = 0;
    for (int i = 0; i < task_count; i++) {
        if (task_pool[i].state == TASK_READY) ready_count++;
    }
    if (ready_count >= 3) {
        sys_mode = MODE_OVERLOAD;
    } else {
        sys_mode = MODE_NORMAL;
    }
}

/* ── MCS Degradation ──────────────────────────────── */
static void apply_mcs_policy(void) {
    if (sys_mode != MODE_OVERLOAD) {
        for (int i = 0; i < task_count; i++) {
            task_pool[i].period_ms = task_pool[i].base_period_ms;
            if (task_pool[i].state == TASK_SUSPENDED)
                task_pool[i].state = TASK_BLOCKED;
        }
        return;
    }
    for (int i = 0; i < task_count; i++) {
        TCB *t = &task_pool[i];
        if (t->asil == ASIL_QM) {
            t->state = TASK_SUSPENDED;
        } else if (t->asil == ASIL_A) {
            t->period_ms = t->base_period_ms * 2;
        }
    }
}

/* ── Round Robin Picker ───────────────────────────── */
static int pick_round_robin(void) {
    for (int i = 0; i < task_count; i++) {
        int idx = (rr_index + i) % task_count;
        if (task_pool[idx].state == TASK_READY) {
            rr_index = (idx + 1) % task_count;
            return idx;
        }
    }
    return -1;
}

/* ── Fixed Priority Picker ────────────────────────── */
static int pick_fixed_priority(void) {
    int best = -1;
    for (int i = 0; i < task_count; i++) {
        if (task_pool[i].state != TASK_READY) continue;
        if (best == -1 ||
            task_pool[i].priority > task_pool[best].priority) {
            best = i;
        }
    }
    return best;
}

/* ── Rate Monotonic Picker ────────────────────────── */
static int pick_rate_monotonic(void) {
    int best = -1;
    for (int i = 0; i < task_count; i++) {
        if (task_pool[i].state != TASK_READY) continue;
        if (best == -1 ||
            task_pool[i].period_ms < task_pool[best].period_ms) {
            best = i;
        }
    }
    return best;
}

/* ── EDF Picker ───────────────────────────────────── */
static int pick_edf(void) {
    int best = -1;
    uint32_t earliest = 0xFFFFFFFF;
    for (int i = 0; i < task_count; i++) {
        if (task_pool[i].state != TASK_READY) continue;
        uint32_t abs_deadline = task_pool[i].next_run +
                                task_pool[i].deadline_ms;
        if (abs_deadline < earliest) {
            earliest = abs_deadline;
            best = i;
        }
    }
    return best;
}

/* ── MCS Picker ───────────────────────────────────── */
static int pick_mcs(void) {
    int best = -1;
    for (int i = 0; i < task_count; i++) {
        if (task_pool[i].state != TASK_READY) continue;
        if (best == -1) { best = i; continue; }
        if (task_pool[i].asil > task_pool[best].asil) {
            best = i;
        } else if (task_pool[i].asil == task_pool[best].asil &&
                   task_pool[i].priority > task_pool[best].priority) {
            best = i;
        }
    }
    return best;
}

/* ── kernel_tick ──────────────────────────────────── */
void kernel_tick(void) {
    tick_count++;

    /* Unblock tasks whose period has expired */
    for (int i = 0; i < task_count; i++) {
        TCB *t = &task_pool[i];
        if (t->state == TASK_SUSPENDED) continue;
        if (tick_count >= t->next_run && t->state == TASK_BLOCKED) {
            t->state = TASK_READY;
        }
        /* Deadline miss detection */
        if (t->state == TASK_READY &&
            tick_count > t->next_run + t->deadline_ms) {
            t->deadline_misses++;
            if (t->asil == ASIL_D) {
                uart_puts("[CRITICAL] ASIL-D miss: ");
                uart_puts(t->name);
                uart_puts("\n");
            }
        }
    }

    /* Apply MCS policy if needed */
    if (sched_algo == MIXED_CRITICALITY) {
        check_overload();
        apply_mcs_policy();
    }

    /* Pick next task based on algorithm */
    int best = -1;
    switch (sched_algo) {
        case ROUND_ROBIN:       best = pick_round_robin();    break;
        case FIXED_PRIORITY:    best = pick_fixed_priority(); break;
        case RATE_MONOTONIC:    best = pick_rate_monotonic(); break;
        case EARLIEST_DEADLINE: best = pick_edf();            break;
        case MIXED_CRITICALITY: best = pick_mcs();            break;
    }

    /* Run selected task */
    if (best >= 0) {
        TCB *t       = &task_pool[best];
        current_task = best;
        t->state     = TASK_RUNNING;
        t->last_start= tick_count;

        t->function();

        t->run_count++;
        t->next_run  = tick_count + t->period_ms;
        t->state     = TASK_BLOCKED;
        current_task = -1;
    }
}

/* ── kernel_start ─────────────────────────────────── */
void kernel_start(void) {
    uart_puts("[KERNEL] Starting scheduler...\n");
    uart_puts("[KERNEL] Tasks registered: ");
    uart_put_uint(task_count);
    uart_puts("\n\n");

    /* Import systick_init from hal */
    extern void systick_init(void);
    systick_init();

    while (1) {
        __asm__("wfi");
    }
}

/* ── Helpers ──────────────────────────────────────── */
uint32_t   kernel_get_ticks(void)      { return tick_count;        }
TCB*       kernel_get_task(int i)      { return &task_pool[i];     }
int        kernel_get_task_count(void) { return task_count;        }
SystemMode kernel_get_mode(void)       { return sys_mode;          }
void       task_yield(void)            { }
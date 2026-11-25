
#include "sched.h"
#include <limits.h>
#include <string.h>

// FCFS
int sched_select_fcfs(const Process *processes, size_t count) {
    int selected = -1;

    for (size_t i = 0; i < count; ++i) {
        if (processes[i].state != STATE_READY)
            continue;

        if (selected == -1 ||
            processes[i].ready_order <
            processes[selected].ready_order) {
            selected = (int)i;
        }
    }

    return selected;
}

// SJF and SRTF
int sched_select_sjf(const Process *processes, size_t count) {
    int selected = -1;

    for (size_t i = 0; i < count; ++i) {
        if (processes[i].state != STATE_READY)
            continue;

        if (selected == -1 ||
            processes[i].remaining <
                processes[selected].remaining ||
            (processes[i].remaining ==
                processes[selected].remaining &&
             processes[i].ready_order <
                processes[selected].ready_order)) {

            selected = (int)i;
        }
    }

    return selected;
}

// Priority comparison
int sched_priority_before(
    const Process *a, const Process *b
) {
    if (a->current_priority != b->current_priority)
        return a->current_priority < b->current_priority;

    if (a->ready_entered_at != b->ready_entered_at)
        return a->ready_entered_at < b->ready_entered_at;

    return strcmp(a->pid, b->pid) < 0;
}

// Priority scheduling
int sched_select_priority(
    const Process *processes, size_t count
) {
    int selected = -1;

    for (size_t i = 0; i < count; ++i) {
        if (processes[i].state != STATE_READY)
            continue;

        if (selected == -1 ||
            sched_priority_before(
                &processes[i], &processes[selected])) {

            selected = (int)i;
        }
    }

    return selected;
}

// Priority aging
void sched_apply_aging(
    Process *processes, size_t count, int time
) {
    for (size_t i = 0; i < count; ++i) {
        Process *p = &processes[i];

        if (p->state != STATE_READY)
            continue;

        long long total_waiting =
            (long long)p->waiting_time +
            time - p->ready_entered_at;

        long long priority =
            (long long)p->original_priority -
            total_waiting / AGING_INTERVAL;

        if (priority < INT_MIN)
            priority = INT_MIN;

        p->current_priority = (int)priority;
    }
}

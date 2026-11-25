
#ifndef SCHED_H
#define SCHED_H

#include <stddef.h>

#define PID_SIZE 32

typedef enum {
    STATE_NEW,
    STATE_READY,
    STATE_RUNNING,
    STATE_WAITING,
    STATE_TERMINATED
} ProcessState;

typedef enum {
    POLICY_FCFS,
    POLICY_SJF,
    POLICY_SRTF,
    POLICY_PRIORITY,
    POLICY_RR
} SchedulingPolicy;

typedef struct {
    char pid[PID_SIZE];

    int arrival;
    int original_priority;
    int current_priority;

    int *bursts;
    size_t burst_count;
    size_t burst_index;
    int remaining;

    ProcessState state;

    int first_dispatch;
    int completion;
    int waiting_time;
    int ready_entered_at;

    unsigned long ready_order;
} Process;

int sched_load_processes(
    const char *path,
    Process **processes,
    size_t *count
);

void sched_free_processes(
    Process *processes,
    size_t count
);

int sched_run(
    Process *processes,
    size_t count,
    SchedulingPolicy policy
);

int sched_select_fcfs(const Process *processes, size_t count);

int sched_select_sjf(const Process *processes, size_t count);

#define AGING_INTERVAL 5

int sched_select_priority(
    const Process *processes, size_t count
);

int sched_priority_before(
    const Process *a, const Process *b
);

void sched_apply_aging(
    Process *processes, size_t count, int time
);

#endif

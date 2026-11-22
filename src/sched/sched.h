
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

int sched_run_fcfs(Process *processes, size_t count);
#endif

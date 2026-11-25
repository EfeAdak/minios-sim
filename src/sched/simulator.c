
#include "sched.h"
#include <stdio.h>
#include <string.h>

static const char *policy_name(SchedulingPolicy policy) {
    switch (policy) {
        case POLICY_FCFS: return "FCFS";
        case POLICY_SJF:  return "SJF";
        case POLICY_SRTF: return "SRTF";
        default:          return "UNKNOWN";
    }
}

static void enter_ready(
    Process *p,
    int time,
    unsigned long *sequence
) {
    p->state = STATE_READY;
    p->ready_entered_at = time;
    p->ready_order = (*sequence)++;
}

// Process arrivals and completed I/O in PID order.
static void admit_ready(
    Process *processes,
    size_t count,
    int time,
    unsigned long *sequence
) {
    while (1) {
        int selected = -1;

        for (size_t i = 0; i < count; ++i) {
            Process *p = &processes[i];

            int eligible =
                (p->state == STATE_NEW &&
                 p->arrival <= time) ||
                (p->state == STATE_WAITING &&
                 p->remaining == 0);

            if (!eligible)
                continue;

            if (selected == -1 ||
                strcmp(p->pid, processes[selected].pid) < 0) {
                selected = (int)i;
            }
        }

        if (selected == -1)
            break;

        Process *p = &processes[selected];

        if (p->state == STATE_WAITING) {
            ++p->burst_index;
            p->remaining = p->bursts[p->burst_index];
        }

        enter_ready(p, time, sequence);
    }
}

static int select_process(
    const Process *processes,
    size_t count,
    SchedulingPolicy policy
) {
    if (policy == POLICY_SJF || policy == POLICY_SRTF)
        return sched_select_sjf(processes, count);

    return sched_select_fcfs(processes, count);
}

static void print_segment(
    const Process *processes,
    int running,
    int start,
    int end
) {
    if (running == -1)
        printf(" [%d-%d:IDLE]", start, end);
    else
        printf(" [%d-%d:%s]",
               start, end, processes[running].pid);
}

int sched_run(
    Process *processes,
    size_t count,
    SchedulingPolicy policy
) {
    if (processes == NULL || count == 0)
        return -1;

    int time = 0;
    int running = -1;
    int previous_running = -2;
    int segment_start = 0;
    int busy_ticks = 0;

    size_t completed = 0;
    unsigned long ready_sequence = 0;

    printf("Gantt (%s):\n", policy_name(policy));

    while (completed < count) {

        // 1. Arrivals and completed I/O
        admit_ready(processes, count, time,
                    &ready_sequence);

        // 2. SRTF preemption
        if (policy == POLICY_SRTF && running != -1) {
            int candidate =
                select_process(processes, count, policy);

            if (candidate != -1 &&
                processes[candidate].remaining <
                processes[running].remaining) {

                enter_ready(&processes[running],
                            time, &ready_sequence);

                running = -1;
            }
        }

        // 3. Dispatch
        if (running == -1) {
            running =
                select_process(processes, count, policy);

            if (running != -1) {
                Process *p = &processes[running];

                p->waiting_time +=
                    time - p->ready_entered_at;

                if (p->first_dispatch == -1)
                    p->first_dispatch = time;

                p->state = STATE_RUNNING;
            }
        }

        // 4. Gantt chart
        if (running != previous_running) {
            if (previous_running != -2) {
                print_segment(processes,
                              previous_running,
                              segment_start, time);
            }

            segment_start = time;
            previous_running = running;
        }

        // 5. Advance I/O
        for (size_t i = 0; i < count; ++i) {
            Process *p = &processes[i];

            if (p->state == STATE_WAITING)
                --p->remaining;
        }

        // 6. Execute one CPU tick
        if (running != -1) {
            Process *p = &processes[running];

            ++busy_ticks;
            --p->remaining;

            if (p->remaining == 0) {
                ++p->burst_index;

                if (p->burst_index == p->burst_count) {
                    p->state = STATE_TERMINATED;
                    p->completion = time + 1;
                    ++completed;
                } else {
                    p->state = STATE_WAITING;
                    p->remaining =
                        p->bursts[p->burst_index];
                }

                running = -1;
            }
        }

        ++time;
    }

    print_segment(processes, previous_running,
                  segment_start, time);

    printf("\n\n%-8s %7s %8s %11s %11s\n",
           "PID", "Waiting", "Response",
           "Turnaround", "Completion");

    for (size_t i = 0; i < count; ++i) {
        const Process *p = &processes[i];

        printf("%-8s %7d %8d %11d %11d\n",
               p->pid,
               p->waiting_time,
               p->first_dispatch - p->arrival,
               p->completion - p->arrival,
               p->completion);
    }

    printf("CPU utilization: %.2f%%\n",
           100.0 * busy_ticks / time);

    printf("Throughput: %.2f processes/100 ticks\n",
           100.0 * count / time);

    return 0;
}

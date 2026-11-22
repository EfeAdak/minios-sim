
#include "sched.h"
#include <stdio.h>

// Select the process that entered READY first.
static int next_ready(const Process *processes, size_t count) {
    int selected = -1;

    for (size_t i = 0; i < count; ++i) {
        if (processes[i].state != STATE_READY)
            continue;

        if (selected < 0 ||
            processes[i].ready_order <
                processes[selected].ready_order) {
            selected = (int)i;
        }
    }

    return selected;
}

// Print a Gantt chart segment.
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

int sched_run_fcfs(Process *processes, size_t count) {
    if (processes == NULL || count == 0)
        return -1;

    int time = 0;
    int running = -1;
    int previous_running = -2;
    int segment_start = 0;
    int busy_ticks = 0;

    size_t completed = 0;
    unsigned long ready_sequence = 0;

    printf("Gantt (FCFS):\n");

    while (completed < count) {

        // 1. New process arrivals
        for (size_t i = 0; i < count; ++i) {
            Process *p = &processes[i];

            if (p->state == STATE_NEW &&
                p->arrival <= time) {

                p->state = STATE_READY;
                p->ready_entered_at = time;
                p->ready_order = ready_sequence++;
            }
        }

        // 2. Dispatch the next READY process
        if (running == -1) {
            running = next_ready(processes, count);

            if (running != -1) {
                Process *p = &processes[running];

                p->waiting_time +=
                    time - p->ready_entered_at;

                if (p->first_dispatch == -1)
                    p->first_dispatch = time;

                p->state = STATE_RUNNING;
            }
        }

        // 3. Track Gantt chart segments
        if (running != previous_running) {
            if (previous_running != -2) {
                print_segment(
                    processes,
                    previous_running,
                    segment_start,
                    time
                );
            }

            segment_start = time;
            previous_running = running;
        }

        // 4. Advance I/O operations
        for (size_t i = 0; i < count; ++i) {
            Process *p = &processes[i];

            if (p->state != STATE_WAITING)
                continue;

            --p->remaining;

            if (p->remaining == 0) {
                ++p->burst_index;

                p->remaining =
                    p->bursts[p->burst_index];

                p->state = STATE_READY;
                p->ready_entered_at = time + 1;
                p->ready_order = ready_sequence++;
            }
        }

        // 5. Execute one CPU tick
        if (running != -1) {
            Process *p = &processes[running];

            ++busy_ticks;
            --p->remaining;

            if (p->remaining == 0) {
                ++p->burst_index;

                if (p->burst_index ==
                    p->burst_count) {

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

    // 6. Print final Gantt segment
    print_segment(
        processes,
        previous_running,
        segment_start,
        time
    );

    // 7. Calculate performance metrics
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

    printf("Throughput: %.2f processes/tick\n",
           (double)count / time);

    return 0;
}

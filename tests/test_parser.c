
#include "sched.h"
#include <stdio.h>

int main(void) {
    Process *processes = NULL;
    size_t count = 0;

    if (sched_load_processes(
            "tests/sched-io.txt",
            &processes,
            &count) != 0) {
        return 1;
    }

    for (size_t i = 0; i < count; ++i) {
        printf("%s arrival=%d priority=%d bursts=%zu\n",
               processes[i].pid,
               processes[i].arrival,
               processes[i].original_priority,
               processes[i].burst_count);
    }

    sched_free_processes(processes, count);
    return 0;
}

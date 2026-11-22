
#include "sched/sched.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "sched") != 0) {
        fprintf(stderr,
                "Usage: %s sched <input-file>\n",
                argv[0]);
        return 1;
    }

    Process *processes = NULL;
    size_t count = 0;

    if (sched_load_processes(
            argv[2], &processes, &count) != 0) {
        return 1;
    }

    printf("Loaded %zu processes:\n", count);

    for (size_t i = 0; i < count; ++i) {
        printf("%s: arrival=%d priority=%d bursts=%zu\n",
               processes[i].pid,
               processes[i].arrival,
               processes[i].original_priority,
               processes[i].burst_count);
    }

    sched_free_processes(processes, count);
    return 0;
}

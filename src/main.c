
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

    int result = sched_run_fcfs(processes, count);

    sched_free_processes(processes, count);

    return result == 0 ? 0 : 1;
}

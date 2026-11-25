
#include "sched/sched.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    SchedulingPolicy policy = POLICY_FCFS;
    const char *path = NULL;

    if (argc == 3 && strcmp(argv[1], "sched") == 0) {
        path = argv[2];

    } else if (argc == 5 &&
               strcmp(argv[1], "sched") == 0 &&
               strcmp(argv[2], "-a") == 0) {

        if (strcmp(argv[3], "fcfs") == 0) {
            policy = POLICY_FCFS;
        } else if (strcmp(argv[3], "sjf") == 0) {
            policy = POLICY_SJF;
        } else if (strcmp(argv[3], "srtf") == 0) {
            policy = POLICY_SRTF;
        } else {
            fprintf(stderr, "Unknown scheduling policy\n");
            return 1;
        }

        path = argv[4];

    } else {
        fprintf(stderr,
                "Usage: %s sched [-a fcfs|sjf] <input-file>\n",
                argv[0]);
        return 1;
    }

    Process *processes = NULL;
    size_t count = 0;

    if (sched_load_processes(
            path, &processes, &count) != 0) {
        return 1;
    }

    int result = sched_run(processes, count, policy);

    sched_free_processes(processes, count);

    return result == 0 ? 0 : 1;
}

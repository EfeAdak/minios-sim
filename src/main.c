
#include "sched/sched.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

int main(int argc, char *argv[]) {
    SchedulingPolicy policy = POLICY_FCFS;
    const char *path = NULL;
    int quantum = 4;
    int quantum_set = 0;

    if (argc < 3 || strcmp(argv[1], "sched") != 0) {
        goto usage;
    }

    for (int i = 2; i < argc; ++i) {

        if (strcmp(argv[i], "-a") == 0) {
            if (++i >= argc)
                goto usage;

            if (strcmp(argv[i], "fcfs") == 0)
                policy = POLICY_FCFS;
            else if (strcmp(argv[i], "sjf") == 0)
                policy = POLICY_SJF;
            else if (strcmp(argv[i], "srtf") == 0)
                policy = POLICY_SRTF;
            else if (strcmp(argv[i], "priority") == 0)
                policy = POLICY_PRIORITY;
            else if (strcmp(argv[i], "rr") == 0)
                policy = POLICY_RR;
            else {
                fprintf(stderr, "Unknown scheduling policy\n");
                return 1;
            }

        } else if (strcmp(argv[i], "-q") == 0) {
            if (++i >= argc)
                goto usage;

            char *end;
            errno = 0;

            long value = strtol(argv[i], &end, 10);

            if (errno || end == argv[i] ||
                *end != '\0' ||
                value <= 0 || value > INT_MAX) {
                fprintf(stderr, "Invalid quantum\n");
                return 1;
            }

            quantum = (int)value;
            quantum_set = 1;

        } else if (argv[i][0] == '-' || path != NULL) {
            goto usage;

        } else {
            path = argv[i];
        }
    }

    if (path == NULL)
        goto usage;

    if (quantum_set && policy != POLICY_RR) {
        fprintf(stderr, "Quantum requires RR policy\n");
        return 1;
    }

    Process *processes = NULL;
    size_t count = 0;

    if (sched_load_processes(
            path, &processes, &count) != 0) {
        return 1;
    }

    int result = sched_run(
        processes, count, policy, quantum
    );

    sched_free_processes(processes, count);

    return result == 0 ? 0 : 1;

usage:
    fprintf(stderr,
        "Usage: %s sched [-a fcfs|sjf|srtf|priority|rr] [-q quantum] <input-file>\n",
        argv[0]);

    return 1;
}

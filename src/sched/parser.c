
#include "sched.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_int(const char *text, int *value) {
    char *end;
    errno = 0;

    long number = strtol(text, &end, 10);

    if (errno || *end != '\0' ||
        number < INT_MIN || number > INT_MAX) {
        return -1;
    }

    *value = (int)number;
    return 0;
}

void sched_free_processes(Process *processes, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        free(processes[i].bursts);
    }
    free(processes);
}

int sched_load_processes(
    const char *path,
    Process **out,
    size_t *out_count
) {
    if (!path || !out || !out_count) return -1;

    *out = NULL;
    *out_count = 0;

    FILE *file = fopen(path, "r");
    if (!file) {
        perror(path);
        return -1;
    }

    Process *list = NULL;
    size_t count = 0;
    size_t line_no = 0;
    char line[4096];
    const char *sep = " \t\r\n";

    while (fgets(line, sizeof line, file)) {
        ++line_no;

        if (!strchr(line, '\n') && !feof(file)) {
            fprintf(stderr, "Line %zu is too long\n", line_no);
            goto fail;
        }

        char *token = strtok(line, sep);

        if (!token || token[0] == '#') continue;

        Process p = {0};

        if (strlen(token) >= PID_SIZE) goto invalid;
        strcpy(p.pid, token);

        token = strtok(NULL, sep);
        if (!token || parse_int(token, &p.arrival)
            || p.arrival < 0) goto invalid;

        token = strtok(NULL, sep);
        if (!token ||
            parse_int(token, &p.original_priority)) goto invalid;

        p.current_priority = p.original_priority;

        while ((token = strtok(NULL, sep)) != NULL
               && token[0] != '#') {

            int duration;

            if (parse_int(token, &duration) ||
                duration <= 0) goto invalid;

            int *next = realloc(
                p.bursts,
                (p.burst_count + 1) * sizeof *p.bursts
            );

            if (!next) goto invalid;

            p.bursts = next;
            p.bursts[p.burst_count++] = duration;
        }

        if (p.burst_count == 0 ||
            p.burst_count % 2 == 0) goto invalid;

        for (size_t i = 0; i < count; ++i) {
            if (strcmp(list[i].pid, p.pid) == 0)
                goto invalid;
        }

        p.remaining = p.bursts[0];
        p.state = STATE_NEW;
        p.first_dispatch = -1;
        p.completion = -1;

        Process *next = realloc(
            list, (count + 1) * sizeof *list
        );

        if (!next) goto invalid;

        list = next;
        list[count++] = p;
        continue;

    invalid:
        fprintf(stderr,
                "Invalid process at line %zu\n", line_no);
        free(p.bursts);
        goto fail;
    }

    if (ferror(file) || count == 0) {
        fprintf(stderr, "Could not read valid processes\n");
        goto fail;
    }

    fclose(file);

    *out = list;
    *out_count = count;
    return 0;

fail:
    fclose(file);
    sched_free_processes(list, count);
    return -1;
}

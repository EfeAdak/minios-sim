
#include "sched.h"

int sched_select_fcfs(const Process *processes, size_t count) {
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

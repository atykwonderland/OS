#include <stdlib.h>
#include <stdio.h>

#include "cpu.h"
#include "shellmemory.h"
#include "shell.h"

CPU *createCPU() {
    CPU *new = (CPU *)malloc(sizeof(CPU));

    new->IP = 0;
    new->IR = NULL;
    new->quanta = 0;

    return new;
}

int deleteCPU(CPU *cpu) {
    cpu->IR = NULL;
    free(cpu);
    cpu = NULL;

    return 0;
}

int runCPU(CPU *cpu) {
    // runs for the number of steps given (quanta)
    while (cpu->quanta > 0) {
        cpu->IR = mem_get_index(cpu->IP);
        if (!cpu->IR) return -1;

        int errorCode = parseInput(cpu->IR);
        if (errorCode == -1) exit(99);
        cpu->IP++;
        cpu->quanta--;
    }

    return 0;
}
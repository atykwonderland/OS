typedef struct cpu {
    int IP;
    char *IR;
    int quanta;
} CPU;

CPU *createCPU();
int runCPU(CPU *cpu);
int deleteCPU(CPU *cpu);
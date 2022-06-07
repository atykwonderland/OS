#include <stdio.h>
#include <stdlib.h>

#include "pcb.h"

PCB *createPCB(int start, int end){
    PCB *new = malloc(sizeof(PCB));
    
    new->pid = rand() % 100;    // pseudo-random number from 1-100
    new->PC = start;
    new->start  = start;
    new->end = end;
    new->age = end - start + 1;
    new->next = NULL;

    return new;
}

int deletePCB(PCB *pcb) {
    if(pcb->next) pcb->next = NULL;
    free(pcb);
    pcb = NULL;

    return 0;
}
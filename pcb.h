typedef struct pcb {
  int pid;
  int PC;
  int start;
  int end;
  int age;
  struct pcb* next;
}PCB;

PCB *createPCB(int start, int end);
int deletePCB(PCB *pcb);
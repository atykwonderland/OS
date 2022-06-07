#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter.h"
#include "shellmemory.h"
#include "pcb.h"
#include "cpu.h"
#include "shell.h"

PCB *head = NULL; // pointer to first PCB
PCB *tail = NULL; // pointer to last PCB
CPU *cpu;

// parse script file into array of instructions
int parseFile(char contents[], size_t size, char *lines[]) {
	char tmp[600];
	int a,b;							
	int w=0; // lineID

	for(a=0; contents[a]==' ' && a<size; a++);		// skip white spaces

	while(contents[a] != '\n' && contents[a] != '\0' && a<size) {

		for(b=0; contents[a]!='\0' && contents[a] != '\n' && a<size; a++, b++)
			tmp[b] = contents[a];					// extract a line
		tmp[b] = '\0';

		lines[w] = strdup(tmp);
	
		w++;
		
		if(contents[a] == '\0'){
			break;
		}
		a++; 
	}

	return w;
}

// Ready Queue Operations
void addToRQ(PCB *p) {
    if (!head && !tail) {
        head = p;
        tail = p;
    } else {
        tail->next = p;
        tail = p;
    }
}

void addToRQHead(PCB *p) {
    if (!head && !tail) {
        head = p;
        tail = p;
    } else {
		p->next = head;
		head = p;
    }
}

PCB *popFromRQ() {
    PCB *pop = head;
    if (head == tail) {
        head = NULL;
        tail = NULL;
    } else {
        head = head->next;
    }

    return pop;
}

int clearRQ() {
    PCB *curr = head;
    while (curr)
    {
        PCB *next = head->next;
        deletePCB(curr);
        curr = next;
    }
    return 0;
}

int sortRQbyLength() {
	PCB *curr = head;
	PCB *index = NULL;
	int pid, PC, start, end, age;

	if(head == NULL) {  
        return -1;  
    } else {
		while (curr != NULL) {
			index = curr->next;
			while (index != NULL) {
				if ((curr->end - curr->start) > (index->end - index->start)) {
				// store curr in tmp
				pid = curr->pid;
				PC = curr->PC;
				start = curr->start;
				end = curr->end;
				age = curr->age;
				// store index in curr
				curr->pid = index->pid;
				curr->PC = index->PC;
				curr->start = index->start;
				curr->end = index->end;
				curr->age = index->age;
				// store tmp in index
				index->pid = pid;
				index->PC = PC;
				index->start = start;
				index->end = end;
				index->age = age;
				}
				index = index->next;
			}
			curr = curr->next;
		}
	}

	return 0;
}

// Age all jobs and return the lowest age in the RQ
int ageRQ() {
	PCB *curr = head;
	int lowest;

	if(head == NULL) {  
        return -1;  
    } else {
        lowest = curr->age;
		while (curr != NULL) {
		    if (curr->age > 0) curr->age--;
			if (curr->age < lowest) lowest = curr->age;
			
			curr = curr->next;
		}
	}

	return lowest;
}

// Find PCB with minAge and move it to the head of the RQ
void promoteRQ(int minAge) {
	PCB *curr = head;
	PCB *newHead;
	
	if (curr->age != minAge) {
    	while (curr->next != NULL) {
    		if (curr->next->age == minAge) {
    			newHead = curr->next;
    			if (curr->next->next == NULL) {
    				curr->next = NULL;
    			} else {
    				curr->next = curr->next->next;
    			}
                break;
    		}
    		curr = curr->next;
    	}
    	newHead->next = NULL;
    	addToRQHead(newHead);
	}
}

// Run/Exec Command Functionality
int badcommandFileDoesNotExist(){
	printf("%s\n", "Bad command: File not found");
	return 3;
}

int loadingError(){
	printf("%s\n", "Loading error: Please input exec command again");
	return 1;
}

int memoryError() {
    printf("%s\n", "Memory error: Please input exec command again");
	clearRQ();
    deleteCPU(cpu);
	return 1;
}

// Put program script into shell memory, make and return a PCB for it
int load(char *script) {
	FILE *p = fopen(script,"rt");
	char *buffer = NULL;
	size_t size = 0;

	if(p == NULL){
		return badcommandFileDoesNotExist();
	}

	// Get file contents into buffer
	fseek(p, 0, SEEK_END);
	size = ftell(p);
	rewind(p);
	buffer = malloc((size + 1) * sizeof(*buffer));
	fread(buffer, size, 1, p);
	buffer[size] = '\0';
	fclose(p);

	// Parse and store contents to Shell Memory
	char *lines[300];
    int start, end;
	int numLines = parseFile(buffer, size, lines);
    
	int err = mem_store_script(script, numLines, lines, &start, &end);
    if (err == 1) {
        mem_del_range(101,1000);
        return loadingError();
    }
    // Make new PCB for script and add to RQ
    addToRQ(createPCB(start, end));

    return 0;
}

// Scheduler for FCFS, SJF, RR, SJF w AGING
int scheduler(int policy) {
    int quanta, status, lowest;
	cpu = createCPU();

	//if policy is sjf, sort the RQ
	if (policy == 1 || policy == 3) {
		if (sortRQbyLength() != 0) {
			//error
			//mem_del_range(curr->start, curr->end);
			return memoryError();
		}
	}

    PCB *curr = popFromRQ();

    while (curr != NULL) {
        cpu->IP = curr->PC;

        switch (policy) {
			case 0:
				// fcfs
			case 1:
				// sjf after sorting
				cpu->quanta = curr->end - curr->start + 1;
				status = runCPU(cpu);
				if (status == -1) {
					//clean up, return error
					mem_del_range(curr->start, curr->end);
					return memoryError();
				}
				// clean up
				mem_del_range(curr->start, curr->end);
				break;
			
			case 2:
				//rr with quanta = 2
				quanta = ((curr->end - curr->PC) >= 1) ? 2 : 1;
				cpu->quanta = quanta;
				status = runCPU(cpu);
				
				curr->PC = cpu->IP;
				
				if (status==0 && curr->PC <= curr->end) {
					// still code left to run so put back into RQ
					addToRQ(curr);
				} else if (status==0 && curr->PC > curr->end) {
					// clean up
					mem_del_range(curr->start, curr->end);
				} else {
					//clean up, return error
					mem_del_range(curr->start, curr->end);
					return memoryError();
				}
				break;
			
			case 3:
				//sjf with aging after first sort
				cpu->quanta = 1;
				status = runCPU(cpu);
				curr->PC = cpu->IP;
				
				//age
				lowest = ageRQ();
				
				if (lowest != -1) {
					if (lowest >= curr->age) {
						//no promotion
						if (status==0 && curr->PC <= curr->end) {
							addToRQHead(curr);
						} else if (status==0 && curr->PC > curr->end) {
							// clean up
							mem_del_range(curr->start, curr->end);
						} else {
							//clean up, return error
							mem_del_range(curr->start, curr->end);
							return memoryError();
						}
					} else {
						//promote
						curr->next = NULL;
						if (status==0 && curr->PC <= curr->end) {
							addToRQ(curr);
						} else if (status==0 && curr->PC > curr->end) {
							// clean up
							mem_del_range(curr->start, curr->end);
						}  else {
							//clean up, return error
							mem_del_range(curr->start, curr->end);
							return memoryError();
						}
						promoteRQ(lowest);
					}
				} else {
					//curr is the only job left
				    curr->next = NULL;
					if (status==0 && curr->PC <= curr->end) {
						addToRQ(curr);
					} else if (status==0 && curr->PC > curr->end) {
						// clean up
						mem_del_range(curr->start, curr->end);
					}  else {
						//clean up, return error
						mem_del_range(curr->start, curr->end);
						return memoryError();
					}
				}
				break;
		}
        
        curr = popFromRQ();
    }

	clearRQ();
    deleteCPU(cpu);

    return 0;
}
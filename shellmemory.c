#include <stdlib.h>
#include <string.h>
#include <stdio.h>


struct memory_struct{
	char *var;
	char *value;
};

struct memory_struct shellmemory[1000];

// Helper functions
int match(char *model, char *var) {
	int i, len=strlen(var), matchCount=0;
	for(i=0;i<len;i++)
		if (*(model+i) == *(var+i)) matchCount++;
	if (matchCount == len)
		return 1;
	else
		return 0;
}

char *extract(char *model) {
	char token='=';    // look for this to find value
	char value[1000];  // stores the extract value
	int i,j, len=strlen(model);
	for(i=0;i<len && *(model+i)!=token;i++); // loop till we get there
	// extract the value
	for(i=i+1,j=0;i<len;i++,j++) value[j]=*(model+i);
	value[j]='\0';
	return strdup(value);
}


// Shell memory functions

void mem_init() {
	int i;
	for (i=0; i<1000; i++){		
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
}

// Set key value pair
void mem_set_value(char *var_in, char *value_in) {
	
	int i;

	for (i=0; i<1000; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	// Value does not exist, need to find a free spot.
	for (i=0; i<1000; i++){
		if (strcmp(shellmemory[i].var, "none") == 0){
			shellmemory[i].var = strdup(var_in);
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	return;

}

// store lines of script 
int mem_store_script (char *script, int size, char *lines[], int *start, int *end) {
	int j = 0;
	char var[100];

	for (int i=100; i<1000 && j < size; i++){
		if (strcmp(shellmemory[i].var, "none") == 0){	//start storing consecutively from first available
			if (j == 0) *start = i;
			sprintf(var, "%s-%d", script, j);
			shellmemory[i].var = strdup(var);
			memset(var, 0, sizeof(var));
			shellmemory[i].value = strdup(lines[j]);
			j++;
		} 
	}
	*end = *start+j-1;

	if (j != size) {
		return 1;
	}

	return 0;
}

// remove script from shell memory
int mem_del_range (int start, int end) {
	if (start < end && end <= 1000) {
		for (int i = start; i < end; i++) {
			shellmemory[i].var = "none";
			shellmemory[i].value = "none";
		}
	}

	return 0;
}

// get value based on input key
char *mem_get_value(char *var_in) {
	int i;

	for (i=0; i<1000; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){

			return strdup(shellmemory[i].value);
		} 
	}

	return NULL;
}

// get value based on position
char *mem_get_index(int i) {
	if (i < 1000) {
		if (strcmp(shellmemory[i].var, "none") != 0){
			return strdup(shellmemory[i].value);
		}
	} 
	
	return NULL;
}
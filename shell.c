#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "interpreter.h"
#include "shellmemory.h"
#include "kernel.h"

int cannotRedirectToTerminal() {
	printf("%s\n", "Unable to open terminal for reading");
	return 1;
}

// Extract words from the input then call interpreter
int parseInput(char ui[]) {
 
	char tmp[200];
	char *words[100];							
	int a,b;							
	int w=0; // wordID

	for(a=0; ui[a]==' ' && a<1000; a++);		// skip white spaces

	while(ui[a] != '\0' && a<1000) {

		for(b=0; ui[a]!='\0' && ui[a]!=' ' && a<1000; a++, b++)
			tmp[b] = ui[a];						// extract a word
	 
		tmp[b] = '\0';

		words[w] = strdup(tmp);

		w++;
		
		if(ui[a] == '\0'){
			break;
		}
		a++; 
	}

	return interpreter(words, w);
}

// Start of everything
int main(int argc, char *argv[]) {
	printf("%s\n", "Shell version 2.0 Created March 2022");
	help();

	char prompt = '$';  				// Shell prompt
	char userInput[1000];		        // user's input stored here
	int errorCode = 0;					// zero means no error, default

	char *commands[10];					// store up to 10 commands per line of input
	char *command;						// one command token
	char *delim = ";";					// token deliminator
	int c = 0;							// command counter

	//init user input
	for (int i=0; i<1000; i++)
		userInput[i] = '\0';
	
	//init shell memory
	mem_init();

	while(1) {							
		printf("%c ",prompt);

		//BATCH
		if (fgets(userInput, 999, stdin) == NULL) {	                // check if error or EOF
			if (freopen("/dev/tty", "r", stdin) == NULL) {			// revert input source to terminal (return to interactive mode)
				return cannotRedirectToTerminal();
			} else {
				fgets(userInput, 999, stdin); 			            // skip to next command since previous was an error
			}
		}

		//ONELINE
		command = strtok(userInput, delim);							// split input by ; if present
		while (command != NULL)										// continue splitting until end of string
		{
			commands[c] = command;
			c++;
			command = strtok(NULL, delim);
		}
		commands[c] = NULL;

		for (int i = 0; i < c; i++) {								// for each command, from this input
			errorCode = parseInput(commands[i]);
			if (errorCode == -1) exit(99);							// ignore all other errors
			if (errorCode == 2) exit(1);
		}

		c=0;
		memset(userInput, 0, sizeof(userInput));
	}

	mem_init();
	
	return 0;
}
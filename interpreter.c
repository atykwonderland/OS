#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "shellmemory.h"
#include "shell.h"
#include "kernel.h"
#include "pcb.h"

int MAX_ARGS_SIZE = 7;

int help(){

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
echo STRINGVAR		Displays the STRING or the STRING assigned to $VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n \
exec (p1 p2 p3) POLICY	Executes programs with scheduling POLICY\n \
my_ls (PATH)		Displays the contents of PATH\n";
	printf("%s\n", help_string);
	return 0;
}

int quit(){
	printf("%s\n", "Bye!");
	return 2;
}

int badcommand(){
	printf("%s\n", "Unknown Command");
	return 1;
}

int badcommandTooManyTokens(){
	printf("%s\n", "Bad command: Too many tokens");
	return 1;
}

int badcommandCannotReadDirectory() {
	printf("%s\n", "Bad command: Unable to read directory");
	return 1;
}

int badcommandRepeatedProgram() {
	printf("%s\n", "Bad command: Same program was inputted twice");
	return 1;
}

int badcommandUnknownPolicy() {
	printf("%s\n", "Bad command: Unknown policy inputted");
	return 1;
}

int set(char* args[], int size){

	char buffer[1000];
	int i = 3;

	strcpy(buffer, args[2]);				// normal set single token

	if (size > 3) {							// for > 3 tokens, concat into one string and set
		while (i < size) {
			strcat(buffer, " ");
			strcat(buffer, args[i]);
			i++;
		}
	}

	mem_set_value(args[1], buffer);

	return 0;

}

int print(char* var){
	printf("%s\n", mem_get_value(var)); 
	return 0;
}

int run(char* script){
	load(script);
	scheduler(0);
	return 0;
}

int exec(char* args[], int size) {
	for (int i = 1; i < size-1; i++) {
		load(args[i]);
	}
	
	if (strcmp(args[size-1],"FCFS") == 0) {
		scheduler(0);
	} else if (strcmp(args[size-1],"SJF") == 0) {
		scheduler(1);
	} else if (strcmp(args[size-1],"RR") == 0) {
		scheduler(2);
	} else if (strcmp(args[size-1],"AGING") == 0) {
		scheduler(3);
	}
	
	return 0;
}

int echo(char* var){
	int i;
	char *value;

	if ( var[0] == '$') {					// if echoing value of var
		var ++;
		value = mem_get_value(var);
		if (strcmp(value, "Variable does not exist")) {
			printf("%s", value); 			// if var exists, print
		}

	} else {								// otherwise just print input
		printf("%s", var); 
	}
	printf("\n"); 

	return 0;
}

int my_ls(char* path){
	struct dirent **filelist;
	int numFiles;							

	numFiles = scandir(path, &filelist, NULL, alphasort);	// scan given path for all files, sort by ASCII, store in filelist
	if (numFiles < 0) {
		badcommandCannotReadDirectory();
	} else {
		for (int i = 0; i < numFiles; i++) {
			if(strncmp((filelist[i]->d_name), ".", 1)) {	// print all files except secret files
				printf("%s \n", filelist[i]->d_name);
			}
			free(filelist[i]);
		}
		free(filelist);
	}

	return 0;
}

// Interpret commands and their arguments
int interpreter(char* command_args[], int args_size){
	int i;

	if ( args_size < 1 ) {
		return badcommand();
	} else if ( args_size > MAX_ARGS_SIZE ) {
		return badcommandTooManyTokens();
	}


	for ( i=0; i<args_size; i++){ 				//strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}

	if (strcmp(command_args[0], "help")==0) {
	    //help
	    if (args_size != 1) return badcommand();
	    return help();
	
	} else if (strcmp(command_args[0], "quit")==0) {
		//quit
		if (args_size != 1) return badcommand();
		return quit();

	} else if (strcmp(command_args[0], "set")==0) {
		//set
		if (args_size < 3) return badcommand();	
		return set(command_args, args_size);
	
	} else if (strcmp(command_args[0], "print")==0) {
		if (args_size != 2) return badcommand();
		return print(command_args[1]);
	
	} else if (strcmp(command_args[0], "run")==0) {
		if (args_size != 2) return badcommand();
		return run(command_args[1]);
	
	} else if (strcmp(command_args[0], "exec")==0) {
		if (args_size < 3 || args_size > 5) return badcommand();
		// check for repeated input programs
		if (args_size > 3) {
			if (strcmp(command_args[1],command_args[2]) == 0) return badcommandRepeatedProgram();
			if (args_size > 4) {
				if (strcmp(command_args[1],command_args[3]) == 0 
					|| strcmp(command_args[2],command_args[3]) == 0) {
					return badcommandRepeatedProgram();
				}
			}
		}
		if (strcmp(command_args[args_size-1],"FCFS")==1 &&
			strcmp(command_args[args_size-1],"SJF")==1 &&
			strcmp(command_args[args_size-1],"RR")==1 &&
			strcmp(command_args[args_size-1],"AGING")==1 ) {
				return badcommandUnknownPolicy();
		}

		return exec(command_args, args_size);

	} else if (strcmp(command_args[0], "echo")==0) {
		if (args_size != 2) return badcommand();
		return echo(command_args[1]);

	} else if (strcmp(command_args[0], "my_ls")==0) {
		if (args_size > 2) return badcommand();
		if (args_size == 1) return my_ls("./");
		return my_ls(command_args[1]);

	} else return badcommand();
}
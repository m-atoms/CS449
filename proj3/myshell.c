/*******************************
 * Author: Michael Adams
 * Last Edit: 4/10/18
 ******************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/times.h>

// prototype functions (should be in a header file)
void parse_input(char *, char* *);
void run_cmds(char* *);

int main() {
	// declare relevant vars
	char input[100]; //array for user input
	char* cmds[5]; //parse user input into commands (this is actually an array of pointers)
	int status = 0; //this will be the function return value to check if success or fail
	// vars for times()
	struct tms t; //struct used by times()
	int tics_per_second;
  	tics_per_second = sysconf(_SC_CLK_TCK);
	
	// loop until user chooses to exit
	while(1) {
		freopen("/dev/tty", "a", stdout);
		
		// get user input
		printf("Shell $ ");
		fgets(input, (int)(sizeof(input)/sizeof(char)), stdin);

		// skip loop to prevent seg fault if user enters nothing
		if (input[0] == '\n') {
			continue;
		}

		// parse user input
		parse_input(input, cmds);
		
		// 1) Exit shell
		if (!strncmp(cmds[0], "exit", 4)) {
			exit(0);
		}
		else if (!strncmp(cmds[0], "time", 4) && !strncmp(cmds[1], "exit", 2)) {
			printf("ERROR: timing information not available for exit command - run without 'time' to exit shell\n");
		}
		// 2) Internal cd - validate input
		else if (!strncmp(cmds[0], "cd", 2)) {
			status = chdir(cmds[1]);
			if (status != 0) {
				printf("ERROR: directory not found\n");
			}
		}
		else if (!strncmp(cmds[0], "time", 4) && !strncmp(cmds[1], "cd", 2)) {
			printf("ERROR: timing information not available for cd command - run without 'time' to change directory\n");
		}
		// if no errors in user input, run commands
		else {
			// track time info if user enters "time"
			if (!strncmp(cmds[0], "time", 4)) {
				times(&t);
				run_cmds(cmds);
				//times(&t);
				printf("            utime           stime\n");
    			printf("parent:    %f        %f\n", ((double) t.tms_utime)/tics_per_second, ((double) t.tms_stime)/tics_per_second);
    			printf("child:     %f        %f\n", ((double) t.tms_cutime)/tics_per_second, ((double) t.tms_cstime)/tics_per_second);
			}
			else {
				run_cmds(cmds);
			}
		}
	}
	return 0;
}

// take user input and parse into list of cmds and arguments
void parse_input(char *input, char* *cmds) {
	// check user input for append or redirect
	int redirect = 0; //flag to check if user wants to redirect output to file
	int append = 0; //flag to check if user wantes to append to file
	int delay = 0; //delay incrementing value

	// set flag
	if (strstr(input, " > ")) {
		redirect = 1;
	}
	if (strstr(input, " >> ")) {
		append = 1;
	}
	
	// tokenize the user input
	*cmds = strtok(input, " \t\n()<>|&;");
	
	while(*cmds != NULL) {
		// MYSTERY
		if ((redirect || append) || !delay) {
			*cmds++; // increment to next pointer
		}

		// reset delay
		if(delay){
			delay = 0;
		}
		
		// read next token
		*cmds = strtok(NULL, " \t\n()<>|&;");
		
		if (*cmds == NULL) {
			break;
		}

		// redirect or append output
		if (redirect && strstr(*cmds, ".")) {
			freopen(*cmds, "w", stdout);
			redirect = 0;
			delay = 1;
		}
		else if (append && strstr(*cmds, ".")) {
			freopen(*cmds, "a", stdout);
			append = 0;
			delay = 1;
		}
	}
	*++cmds = "\0";
}

// run user commands
void run_cmds(char* *cmds) {
	// skip "time" arg if necessary
	if (!strncmp(cmds[0], "time", 4)) {
		cmds++;
	}

	// create child process and run command
	if (fork() == 0) {
		if (execvp(cmds[0], cmds) < 0) {  
			printf("ERROR: unable to execute child process\n");
			exit(1); //this prevents an issue where invalid cmd prevents exit() from exiting shell
		}
	}
	else {
		int status;
		wait(&status);
	}
}
/*
 * The Tiny Torero Shell (TTSH)
 *
 *Authors: Julia Paley and Tyler Kreider
 *Date: 11/29/2021

 * This program implements a Linux shell program which supports running
 * commands in the foreground, running commands in the background, exiting
 * shell with the built-in exit command, and displaying a list of recently
 * entered command lines by the user.
 * 
 */

#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>

#include "parse_args.h"
#include "history_queue.h"


int execute(char **argv, int back, char cmdline[]);
extern char *pastCmd(char *cmdline);
void run_command(char **argv, int back);
void sigchld_handler(__attribute__ ((unused)) int signum);
int main() { 
	struct sigaction sa;
	sa.sa_handler = sigchld_handler;
	sa.sa_flags = SA_NOCLDSTOP;     // don't call handler on child pause/resume
	sigaction(SIGCHLD, &sa, NULL);

	while(1) {
		// (1) print the shell prompt
		fprintf(stdout, "ttsh> ");  
		fflush(stdout);

		// (2) read in the next command entered by the user
		char cmdline[MAXLINE];
		if ((fgets(cmdline, MAXLINE, stdin) == NULL)
				&& ferror(stdin)) {
			// fgets could be interrupted by a signal.
			// This checks to see if that happened, in which case we simply
			// clear out the error and restart our loop so it re-prompts the
			// user for a command.
			clearerr(stdin);
			continue;
		}

		/*
		 * Check to see if we reached the end "file" for stdin, exiting the
		 * program if that is the case. Note that you can simulate EOF by
		 * typing CTRL-D when it prompts you for input.
		 */
		if (feof(stdin)) {
			fflush(stdout);
			exit(0);
		}
		char *argv[MAXARGS];
		int back = parseArguments(cmdline, argv); //parse cmdline into correct format
		execute(argv, back, cmdline); //call to function that executes the commandline argument
	}
	return 0;
}

/**
 *Executes history, exit, and functions in the queue. Determines how to run other commands through a call to run_command.
 *
 *@param  **argv The parsed commandline argument
 *@param back The value returned by parseArguments
 *@param cmdline The string entered in by the user
 *
 * @return 1 if user entered history or a value not in the queue, else return 0
 */
int execute(char **argv, int back, char cmdline[]){
	if(strncmp(argv[0] ,"!", 1) == 0){ //if user wants to run a value from the queue
		memmove(cmdline, cmdline+1, strlen(cmdline)); //remove the "!" the user entered from cmdline
		if((cmdline = pastCmd(cmdline)) != 0){ //if the value entered is in the queue
			back = parseArguments(cmdline, argv); //parse cmdline into correct format 
		}
		else{
			return 1; 
		}
	}   

	if((strcmp(argv[0], "exit") == 0)){ //if user enters "exit"
		printf("Exiting. Have a good day!\n");
		exit(EXIT_SUCCESS);

	}
	else if((strcmp(argv[0], "history") == 0)){ //if user enters "history"
		add_queue(cmdline); //add command to the queue
		print(); //print the queue
		return 1;

	}
	add_queue(cmdline); //add command to the queue
	run_command(argv, back); //function that runs commands other than history and exit
	return 0;
}

/**
 *Runs commands other than history and exit. Exits if shell is unable to run the command.
 *
 *@param **argv The parsed commandline argument
 *@param back The value returned by parseArguments
 *
 */

void run_command(char **argv, int back){
	pid_t pid1;
	int status;
	pid1 = fork();//create a new process
	if(pid1 == 0){ //if child
		if(execvp(argv[0], argv) == -1){//if execvp returns an error
			printf("ERROR: Command not found: %s", *argv);
			exit(EXIT_FAILURE);
		}
	}	

	else if (back == 0){ //run command in the foreground
		waitpid(pid1, &status, 0);//parent waits on the child

	}
	else { //else run command in the background
		waitpid(pid1, &status, WNOHANG); 
	}


}
/**
 *Function that is called when background process terminates, allowing the parent to call waitpid to reap the child.
 *
 *@param signum Signal id number
 */
void sigchld_handler(__attribute__ ((unused)) int signum) {
	int status; 
	pid_t pid;

	while( (pid = waitpid(-1, &status, WNOHANG)) > 0) {

	}
}

/**
 *Function that determines if the past command in the queue is still in the queue.Prints an error message if the command is not in history.
 *
 *@param *cmdline The unparsed string entered in by the user without the "!"
 *
 * @return Cmdline if in the queue, otherwise return 0
 */
extern char *pastCmd(char *cmdline){
	unsigned int cmd_index = atoi(cmdline); //convert cmdline to an int
	if(search(cmd_index, cmdline) == 0){ //if not in queue
		printf("Error: %d is not in history\n", cmd_index);
	}
	else{ //if in queue 
		return cmdline;
	}
	return 0;


}

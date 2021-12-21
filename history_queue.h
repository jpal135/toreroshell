/**
 * Header file for circular queue library for tracking the history of commands
 * entered in a shell.
 */
#ifndef __HISTQUEUE_H__
#define __HISTQUEUE_H__

#include "parse_args.h"

#define MAXHIST 10   // max number of commands in history list


// Each HistoryEntry will hold one command in the history.
struct HistoryEntry {              
    unsigned int cmd_num;
    char cmdline[MAXLINE]; // command line for this process
};

// You can use "HistoryEntry" instead of "struct HistoryEntry"
typedef struct HistoryEntry HistoryEntry;


// TODO: Put the function prototypes for your history queue functions here
void add_queue(char *new_entry);
void print();
int search(unsigned int cmd, char *new_cmd);
#endif

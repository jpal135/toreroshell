/*
 * The Tiny Torero Shell (TTSH)
 *
 * Add your top-level comments here.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "history_queue.h"

// history will be the array used by our circular queue of HistoryEntry items.
// Note that the "static" keyword means that this global variable is accessible
// only within this file.


static HistoryEntry history[MAXHIST]; 

static int queue_start = 0; // the index where queue begins

static int queue_next = 0;  // The index where we will insert next item.

static int queue_size = 0;  // The number of items in the queue.

static int num_entry = 0; //The entry number.

//This function adds a new command into the history command queue that holds
//only 10 entries. It uses circular queue to handle more than 10 entries. 
//@param *new_entry is the new command we want to add
void add_queue(char *new_entry){
	if (queue_next < MAXHIST) { //if queue not full
		history[queue_next].cmd_num = num_entry;
		strncpy(history[queue_next].cmdline, new_entry, MAXLINE);
		queue_next++; //increment queue_next
	} 
	else { //if queue is full
		queue_next = 0; //reset queue_next to 0
		history[queue_next].cmd_num = num_entry;
		strncpy(history[queue_next].cmdline, new_entry, MAXLINE);
		queue_next++; //increment queue_next

	}
	if(queue_size >= MAXHIST){	//if queue_size exceeds 10
		if  (queue_start < MAXHIST) { //update queue_start only if queue is full
			queue_start++;
		} 
		else {
			queue_start = queue_next; //update queue_start
		}
	}
	else {
		queue_size++;
	}
	num_entry++;
}

//Function that prints out the queue.
void print(){	
	for(int i = 0, j = 0; i < queue_size; i++){//loop through the queue up to queue_size
		if(queue_start+i < MAXHIST){ //check if has reached the end of queue
			printf("%d\t%s", history[queue_start + i].cmd_num, history[queue_start + i].cmdline);
		}
		else{ //else prints using j
			printf("%d\t%s", history[j].cmd_num, history[j].cmdline);
			j++;
		}	
	}

}

/**
 *Searches the queue to find if value user entered is in it.
 *
 *@param cmd The queue number the user entered.
 *@param *new_cmd The command associated with the queue number
 *
 * @return 1 if in the queue, else return 0
 */
int search(unsigned int cmd, char *new_cmd){
	for(int i = 0; i < MAXHIST; i++){ //loop through queue
		if(cmd == history[i].cmd_num){ //if command numbers match
			strcpy(new_cmd, history[i].cmdline);
			return 1;
		}
	}
	return 0;
}

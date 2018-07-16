/*******************************
 * Author: Michael Adams
 * Last Edit: 3/27/18
 ******************************/

#include <stdio.h>
#include "mymalloc.h"

// Node struct def - 16 bytes (32 bit build) or 24 bytes (64 bit build)
typedef struct Node {
	int size; //size of the data associated with this node
	char used; //availability of data [0 = free, 1 = in use]
	struct Node *next; //reference to next node
	struct Node *previous; //reference to previous node
}Node;

// declare global vars
Node *first; //first node in list
Node *last; //last node in list
Node *curr; //current node in list
void *base_brk; //base value of brk to be saved after first allocation

// Allocate space using Worst Fit, increase heap if necessary
void *my_worstfit_malloc(int size) {
	Node *loop_node; //use during worst fit search
	Node *worst; //this will point to node at beginning of largest free space
	Node *temp_node; //used when for splitting free space into two nodes on allocation
	int free_size; //remaining free size after splitting worst node
	// if first allocation - create list
	if (curr == NULL) {
		first = create_node(size);

		// check to make sure allocation didnt fail
		if (first == NULL) {
			return NULL; //this means malloc will return NULL which means allocation failed
		}
		// if allocation didnt fail, assign references to get the LL going
		curr = first;
		last = first; // "... and the first shall be last"
		base_brk = first;
	}
	else { // perform worst fit search to see if data will fit anywhere
		loop_node = first;
		worst = first; //start from beginning of list
		
		do { //check each node to see if larger than current largest
			if (loop_node->size > worst->size && loop_node->used == 0) {
				worst = loop_node;
			}
			loop_node = loop_node->next;
		}while(loop_node != NULL);
		// after finding largest available space, check to see if big enough for data and new node
		// have to check used again bc worst is set to first by default and might be in use
		if (worst->size >= (sizeof(Node) + size) && worst->used == 0) {
			// put data into space
			worst->used = 1;
			free_size = worst->size - (sizeof(Node) + size);
			worst->size = size;

			temp_node = worst + (sizeof(Node) + size); //this should do pointer math to set temp address to right after new data
			temp_node->size = free_size;
			temp_node->used = 0;
			temp_node->next = worst->next;
			temp_node->previous = worst;
			curr = worst;
		}
		else { // create new node and increment brk
			temp_node = create_node(size);
			// initialize values
			last->next = temp_node;
			temp_node->previous = last;
			last = temp_node;
			curr = temp_node;
		}
	}
	return (void *)((void *)curr + sizeof(Node)); //hopefully this returns middle of new allocation
}

// Create node used in heap LL, return void pointer (automatically cast as Node pointer on assignment)
void *create_node(int size) {
	// allocate space for Node and data - cast sbrk() to void pointer bc warning
	void *new_space = (void *)sbrk(sizeof(Node) + size);

	// sbrk() fail returns -1, so return NULL for Node
	if (new_space == NULL) {
		return NULL;
	}

	// initialize a new node
	Node *new_node = new_space; //pointer to new node assigned to beginning of data space
	new_node->size = size; //size of data allocation request
	new_node->used = 1; //data is in use
	new_node->next = NULL; //this is currently the end of the list
	new_node->previous = NULL; //default value, if not first allocation this needs to be set

	return new_node;
}

// free 
void my_free(void *ptr) {
	// check to make sure ptr is legit
	// adjust ptr down to start of node
	Node *freed_node = ptr - sizeof(Node);
	Node *temp_node;

	// set node free
	freed_node->used = 0;
	
	// if next node is free merge next with freed
	if(freed_node->next != NULL && (freed_node->next)->used == 0) {
		temp_node = freed_node->next->next; //might be NULL
		freed_node = merge_node(freed_node, freed_node->next);

		if (freed_node == first) {
			freed_node->next == NULL;
		}
		else {
			freed_node->next = temp_node;
		}
	}
	
	// if previous node is free merge prev with freed
	if(freed_node->previous != NULL && freed_node->previous->used == 0){
		temp_node = freed_node->previous->previous;
		freed_node = merge_node(freed_node, freed_node->previous);

		if (freed_node == last) {
			//freed_node->next == NULL;
		}
		freed_node->previous = temp_node;
	}
	
	// decrement brk if freeing node at top of heap
	if ((freed_node->next == NULL || freed_node == first) && freed_node != NULL) {
		sbrk(- (freed_node->size + sizeof(Node)));

		if (first == last && last == freed_node) {
			curr = first = last = NULL;
			//print_heap();
			return;
		}
		else {
			last = freed_node->previous;
			if((void *)last == base_brk) {
				first = last = curr = NULL;
			}
			if (last != NULL) {
				last->next=NULL;
			}
			//print_heap();
			return;
		}
	}
}

// Merge two nodes passed
Node *merge_node(Node *a, Node *b){
	void * reset_brk = (void *)sbrk(0);
	
	// early exits if it can't pull it off
	if(a == NULL || b == NULL) {
		return a;
	}
	
	// this should be true if a is last node and b is next
	if(a != NULL && b == NULL) {
		//nvm
	}

	// check if a = last and b = last->prev
	if (a == last) {
		b->size = a->size + b->size + sizeof(Node);
		b->next = NULL;
		a= b;
		last = a;
		return a;
	}

	if (a->previous == NULL) {
		a->size = a->size + b->size + sizeof(Node);
		a->next = NULL;
		first = a;
		return a;
	}

	// merge the nodes
	a->size = a->size + b->size + sizeof(Node);
	a->next = b->next;
	if(b == last) { //was using if b == last (this means that b is last)
		a = last;
		curr = a;
	}
	else { // go to a's new next node and set a to be its previous node
		a->next->previous = a;
	}
	return a;
}

void print_heap() {
	Node *loop_node;
	loop_node = first;
	int count = 0;
	do {
		printf("Address: %p Used: %d Size: %d Prev: %p Next: %p\n", loop_node, loop_node->used, loop_node->size, loop_node->previous, loop_node->next);
		loop_node = loop_node->next;
		count++;
	}while(loop_node != NULL && count < 7);
}
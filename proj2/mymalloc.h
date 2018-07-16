/*******************************
 * Author: Michael Adams
 * Last Edit: 3/27/18
 ******************************/

void *my_worstfit_malloc(int);

// helper function for allocating new nodes
void *create_node(int);

void my_free(void *);

struct Node *merge_node(struct Node *, struct Node *);

void print_heap();
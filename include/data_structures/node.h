#ifndef __node_h__
#define __node_h__

#include <malloc.h>

typedef struct _node {
	int byte;
		/* if byte <0, then this is a parent node (left/right pointers valid)
		 * if byte >=0, then this is a leaf node (left/right pointers NOT valid)
		 */
	long long unsigned int weight;
	struct _node *left;
	struct _node *right;
	struct _node *next;  /* Linked list pointer for tree construction */
} node;

/* Node creation / destruction */
node* new_node(int byte, long long unsigned int weight);
void delete_node(node *n);

long long unsigned int get_weight(node *n);
int compare_nodes(node *a, node *b);

#endif
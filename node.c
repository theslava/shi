#include "node.h"
#include "metric.h"

/* Node creation / destruction */
node* new_node(int byte, long long unsigned int weight) {
	/* TODO: Allocate and initialize a node */
	return NULL;
}

void delete_node(node *n) {
	/* TODO: Free a single node (does not free children) */
	if (n != NULL) {
		free(n);
	}
}

long long unsigned int get_weight(node *n) {
	return n->weight;
}

int compare_nodes (node *a, node *b) {
	int result = a->weight - b->weight;
	if (result < 0) return -1;
	else if (result > 0) return 1;
	else return 0;
}
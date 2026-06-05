#include "data_structures/node.h"
#include "utils/metric.h"

/* Node creation / destruction */
node* new_node(int byte, long long unsigned int weight) {
    node* n = (node*)malloc(sizeof(node));
	if (n != NULL) {
        n->byte = byte;
        n->weight = weight;
        n->left = NULL;
        n->right = NULL;
	}
    return n;
}

void delete_node(node *n) {
	if (n != NULL) {
		free(n);
}
}

long long unsigned int get_weight(node *n) {
	return n->weight;
}

int compare_nodes (node *a, node *b) {
	if (a->weight < b->weight) return -1;
	else if (a->weight > b->weight) return 1;
	else return 0;
}

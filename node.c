#include "node.h"
#include "metric.h"

long long unsigned int get_weight(node *n) {
	return n->weight;
}

int compare_nodes (node *a, node *b) {
	int result = a->weight - b->weight;
	if (result < 0) return -1;
	else if (result > 0) return 1;
	else return 0;
}
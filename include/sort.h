#ifndef SORT_H
#define SORT_H

#include <stdlib.h>
#include "node.h"

//compare two nodes by weight
static inline int compare_nodes_weight (const node *a, const node *b) {
	if (a->weight < b->weight) return -1;
	else if (a->weight > b->weight) return 1;
	else return 0;
}

//swap the elements in the given array
static inline void swap_nodes(node **x, node **y) {
	node *temp = *x;
	*x = *y;
	*y = temp;
}

//compare nodes for heapsort
static inline int compare_nodes (node *a, node *b) {
	return compare_nodes_weight (a, b);
}

//swap the elements in the given array
static inline void swap_strings(char *x, char *y) {
	char *temp = x;
	x = y;
	*y = temp;
}
static inline void swap_heap(node **heap, int count) {
	node *tmp = heap[0];
	heap[0] = heap[count];
	heap[count] = tmp;
}

static void heapify(node **heap, int count, int root) {
	int left = 2 * root + 1;
	int right = 2 * root + 2;

	//we have no kids, return
	if (left > count) return;

	//if we have a left child, heapify the subtree
	if (left <= count)	heapify(heap, count, left);

	//if we have a right child, heapify the subtree
	if (right <= count) heapify(heap, count, right);

	//we have two heapified subtrees, let's test the kids
	if (left == count) {
		//we have only the left child
		if (compare_nodes(heap[left], heap[root]) == 1) {
			node* tmp = heap[left];
			heap[left] = heap[root];
			heap[root] = tmp;
		}
	}
	else {
		if (compare_nodes(heap[left], heap[right]) == 1) {
			if (heap[left] > heap[root]) {
				node* tmp = heap[left];
				heap[left] = heap[root];
				heap[root] = tmp;
			}
		}
		else {
			if (compare_nodes(heap[right], heap[root]) == 1) {
				node* tmp = heap[right];
				heap[right] = heap[root];
				heap[root] = tmp;
			}
		}
	}
}

void heapsort(node **heap) {
	int i = 256;
	while (i > 0) {
		heapify (heap, i, 0);
		swap_heap(heap, i);
		i--;
	}
}

#endif /* SORT_H */


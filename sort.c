#include "sort.h"

void swap(node **heap, int count) {
	node *tmp = heap[0];
	heap[0] = heap[count];
	heap[count] = tmp;
}

void heapify(node **heap, int count, int root) {
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
		swap(heap, i);
		i--;
	}
}
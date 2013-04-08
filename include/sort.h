/*
 *      sort.h
 *
 *      Copyright 2007 Vyacheslav Goltser <slavikg@gmail.com>
 *
 *      This program is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __sort_h__
#define __sort_h__

#include "node.h"

inline void swap(node **heap, int count) {
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

#endif

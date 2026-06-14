/* Released to the public domain — No rights reserved. */

#ifndef __sort_h__
#define __sort_h__

#include "data_structures/node.h"

void swap(node** heap, int count);
void heapify(node** heap, int count, int root);
void heapsort(node** heap, int count);

/* Sort an array of node pointers by weight (ascending) */
void sort_nodes_by_weight(node** nodes, int count);

#endif

#ifndef LIST_H
#define LIST_H
#include <stdlib.h>
#include "node.h"
#include "metric.h"

typedef struct _list {
	node *head;
} list;

list *new_list_from_array(node *array) {
	list *ret = (list *)malloc(sizeof(list));
	ret->head = array;
	if (!ret->head) {
		free(ret);
		return NULL;
	}
	node *current = ret->head;

	// Set left of head to NULL
	current->left = NULL;

	// Iterate through the array to link nodes
	for (int i = 0; i < 255; i++) {
		array[i].right = &(array[i + 1]);
		array[i + 1].left = &(array[i]);
	}
	
	// Set right of last node to NULL
	array[255].right = NULL;

	return ret;
}

//insert in sorted order
void insert_node(list *l, node *n, metric *m) {
	if (!l || !n || !m) return;
	node *trav = l->head;
	if (!trav) {
		l->head = n;
		return;
	}
	
	while (get_weight(n) > get_weight(trav)) {
		if (!trav->right) {
			// Append to the end
			trav->right = n;
			n->left = trav;
			return;
		}
		trav = trav->right;
	}

	// Insert between trav->left and trav
	node *prev = trav->left;
	n->right = trav;
	n->left = prev;

	if (prev) {
		prev->right = n;
	}
	else {
		l->head = n;
	}
	trav->left = n;
}

node *pop(list *l) {
	if (!l || !l->head) return NULL;

	node *ret = l->head;
	l->head = ret->right;
	if (l->head) {
		l->head->left = NULL;
	}

	// Clean up node pointers to avoid dangling references
	ret->left = NULL;
	ret->right = NULL;

	return ret;
}

#endif /* LIST_H */


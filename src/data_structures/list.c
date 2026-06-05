#include "data_structures/list.h"
#include <stdlib.h>
#include <stddef.h>

/* List creation / destruction */
list* new_list(void) {
	list *l = (list*)malloc(sizeof(list));
	if (l != NULL) {
		l->head = NULL;
		l->count = 0;
	}
	return l;
}

void delete_list(list *l) {
	/* TODO: Free all nodes in the list */
	if (l != NULL) {
		free(l);
	}
}

list *new_list_from_array(node** array, int count) {
	list *ret = new_list();
	if (!ret || !array || count <= 0) return ret;
	
	/* Link the nodes in sorted order (assumes array is already sorted by weight) */
	ret->head = array[0];
	ret->count = count;
	
	for(int i = 0; i < count - 1; i++) {
		array[i]->right = array[i + 1];
	}
	if (count > 1) {
		array[count - 1]->right = NULL;
	}
	
	/* Set left pointers for doubly-linked traversal */
	for(int i = 1; i < count; i++) {
		array[i]->left = array[i - 1];
	}
	if (count > 0) {
		array[0]->left = NULL;
	}
	
	return ret;
}

//insert in sorted order
void insert_node(list* l, node* n, metric* m) {
	(void)m;
	node * trav = l->head;
	
	/* If list is empty or new node has less weight than head */
	if (trav == NULL || get_weight(n) <= get_weight(trav)) {
		n->right = trav;
		n->left = NULL;
		if (trav != NULL) {
			trav->left = n;
		}
		l->head = n;
		l->count++;
		return;
	}
	
	while (get_weight(n) > get_weight(trav)) {
		trav = trav->right;
	}
	
	/* Insert n between trav->left and trav */
	n->right = trav;
	n->left = trav->left;
	if (trav->left != NULL) {
		trav->left->right = n;
	}
	trav->left = n;
	l->count++;
}

//remove the first element without deleting it
node* remove_node(list *l) {
	node *ret = l->head;
	if (ret == NULL) return NULL;
	
	l->head = ret->right;
	if (l->head != NULL) {
		l->head->left = NULL;
	}
	ret->right = NULL;
	ret->left = NULL;
	l->count--;
	return ret;
}

/* Iteration helpers */
int list_size(const list *l) {
	if (l == NULL) return 0;
	return l->count;
}

node* list_get_head(const list *l) {
	if (l == NULL) return NULL;
	return l->head;
}

list* list_append(list *l, node *n) {
	/* TODO: Append a node to the end of the list */
	if (l == NULL || n == NULL) return l;
	
	if (l->head == NULL) {
		l->head = n;
		n->left = NULL;
		n->right = NULL;
	} else {
		node *trav = l->head;
		while (trav->right != NULL) {
			trav = trav->right;
		}
		trav->right = n;
		n->left = trav;
		n->right = NULL;
	}
	l->count++;
	return l;
}


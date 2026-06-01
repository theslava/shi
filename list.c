#include "list.h"
#include <stdlib.h>
#include <stddef.h>

list * new_list_from_array(node* array) {
	list * ret = (list*)malloc(sizeof(list));
	ret->head = array;
	array[0].left = NULL;
	array[0].right = &(array[1]);
	array[255].left = &(array[254]);
	array[255].right = NULL;
	
	for(int i=1; i < 255; i++) {
		array[i].left = &(array[i-1]);
		array[i].right = &(array[i+1]);
	}
	
	return ret;
}

//insert in sorted order
void insert_node(list* l, node* n, metric* m) {
	node * trav = l->head;
	
	while (get_weight(n) > get_weight(trav)) {
		trav = trav->right;
	}
	
	trav->left = trav->left->right = n;
	n->left = trav->left;
	n->right = trav;
}

//remove the first element without deleting it
node* remove_node(list* l) {
	node * ret = l->head;
	l->head = ret->right;
	ret->left = NULL;
	return ret;
}
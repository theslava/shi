/*
 *      list.h
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

#ifndef __list_h__
#define __list_h__

#include "node.h"

typedef struct _list {
	node *head;
} list;

list * new_list_from_array(node* array) {
	list * ret = (list*)malloc(sizeof(list));
	ret->head = array;
	array[0].left = NULL;
	array[0].right = &(array[1]);
	array[255].left = &(array[254]);
	array[255].left = NULL;
	
	for(int i=1; i < 255; i++) {
		array[i].left = &(array[i-1]);
		array[i].right = &(array[i+1]);
	}
	
	return ret;
}

//insert in sorted order
void insert_node(list* l, node* n, metric* m) {
	node * trav = l->head;
	
	while (get_weight(n,m) > get_weight(trav,m)) {
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
	l->head->left = NULL;
	return ret;
}

#endif

/*
 *      tree.h
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

#ifndef __tree_h__
#define __tree_h__

#include <malloc.h>
#include "node.h"
#include "sort.h"

typedef struct _tree {
	node * root;
	node nodes[512]; // array of nodes
} tree;

tree * new_tree_from_metric(metric * met) {
	int first_node = 0;
	int last_node = 255;
	int next_pnode = 256;
	int i;
	
	tree * ret = (tree*)malloc(sizeof(tree));
	node * node_index[256];
	
	//initialize the array
	for (i = 0; i < 256; i++) {
		ret->nodes[i].left = NULL;
		ret->nodes[i].right = NULL;
		ret->nodes[i].byte = i;
		ret->nodes[i].weight = met->characters[i];
		node_index[i] = &(ret->nodes[i]);
	}
	
	//sort the array
	heapsort(node_index);
	
	//while there are more than 1 nodes
	while (first_node != last_node) {
		//combine the two first nodes under a parent node
		ret->nodes[next_pnode].left = node_index[first_node];
		ret->nodes[next_pnode].right = node_index[first_node+1];
		ret->nodes[next_pnode].weight = get_weight(node_index[first_node]) + get_weight(node_index[first_node+1]);
		ret->nodes[next_pnode].byte = -1;
		//move up the beginning of the 'list'
		first_node++;
		
		//move the nodes up so that the parent can be put into proper place
		for(i = first_node+1; ((i <= last_node) && (get_weight(node_index[i]) < ret->nodes[next_pnode].weight)); i++) {
			node_index[i-1] = node_index[i];
		}
		node_index[i] = node_index[next_pnode++];
	}

	ret->root = node_index[first_node];
	return ret;
}

void delete_tree(tree *del) {
	free(del);
}

#endif

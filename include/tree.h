#ifndef TREE_H
#define TREE_H

#include <stdlib.h>
#include "node.h"
#include "sort.h"
#include "metric.h"

typedef struct _tree {
	node * root;
	node nodes[512]; // array of nodes
} tree;

tree * new_tree_from_metric(metric * met) {
	if (!met) return NULL;

	int first_node = 0;
	int last_node = 255;
	int next_pnode = 256;
	int i;
	
	tree * ret = (tree*)malloc(sizeof(tree));
	if (!ret) return NULL;

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
	if (del) free(del);
}

#endif /* TREE_H */


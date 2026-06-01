#include "tree.h"
#include "metric.h"

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
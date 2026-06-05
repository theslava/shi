#include "data_structures/tree.h"
#include "utils/metric.h"

/* Tree creation / destruction */
tree* new_tree(void) {
	tree *ret = (tree*)malloc(sizeof(tree));
	if (ret != NULL) {
		ret->root = NULL;
	}
	return ret;
}

void delete_tree(tree *del) {
	/* Free dynamically allocated nodes first */
	if (del != NULL && del->root != NULL) {
		free_tree_nodes(del->root);
	}
	free(del);
}

tree *new_tree_from_metric(metric * met) {
	int first_node = 0;
	int last_node = 255;
	int next_pnode = 256;
	int i;

	tree * ret = new_tree();
	node * node_index[256];

	//initialize the array
	for (i = 0; i < 256; i++) {
		ret->nodes[i].left = NULL;
		ret->nodes[i].right = NULL;
		ret->nodes[i].byte = i;
		ret->nodes[i].weight = met->characters[i];
		node_index[i] = &(ret->nodes[i]);
	}

	//sort the array by weight (ascending)
	sort_nodes_by_weight(node_index, 256);

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
		node_index[i] = &(ret->nodes[next_pnode++]);
	}

	ret->root = node_index[first_node];
	return ret;
}

/* Free dynamically allocated nodes in the tree (not the static array) */
void free_tree_nodes(node *root) {
	if (root == NULL) return;
	free_tree_nodes(root->left);
	free_tree_nodes(root->right);
	/* All nodes are stored in the tree's static array, so nothing to free here. */
}
int generate_codes(tree *t, unsigned int codes[256], int code_lengths[256]) {
	(void)t; (void)codes; (void)code_lengths;
	/* TODO: Traverse the Huffman tree to assign codes
	 *
	 * Algorithm:
	 * 1. Start at root with empty code and length 0
	 * 2. For each left edge, append a '0' bit
	 * 3. For each right edge, append a '1' bit
	 * 4. At leaf nodes (byte >= 0), store the accumulated code and length
	 *
	 * Returns the count of symbols with non-zero frequency */
	return 0;
}


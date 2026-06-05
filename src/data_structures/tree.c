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
	free(del);
}

tree *new_tree_from_metric(metric * met) {
	int first_node = 0;
	int last_node = 255;
	int next_pnode = 256;
	int i;

	tree * ret = new_tree();
	if (!ret) return NULL;

	node * node_index[256];

	/* Initialize the array with leaf nodes for each byte value */
	for (i = 0; i < 256; i++) {
		ret->nodes[i].left = NULL;
		ret->nodes[i].right = NULL;
		ret->nodes[i].byte = i;
		ret->nodes[i].weight = met->characters[i];
		node_index[i] = &(ret->nodes[i]);
	}

	/* Sort the array by weight (ascending) */
	sort_nodes_by_weight(node_index, 256);

	/* While there are more than 1 nodes, combine the two smallest */
	while (last_node > first_node) {
		/* Create a parent node */
		ret->nodes[next_pnode].left = node_index[first_node];
		ret->nodes[next_pnode].right = node_index[first_node+1];
		ret->nodes[next_pnode].weight = node_index[first_node]->weight + node_index[first_node+1]->weight;
		ret->nodes[next_pnode].byte = -1;
		first_node++;

		/* Insert the parent node into the sorted position */
		int insert_pos = first_node;
		while (insert_pos < last_node && get_weight(node_index[insert_pos]) < ret->nodes[next_pnode].weight) {
			insert_pos++;
		}
		/* Shift nodes to make room */
		int j = next_pnode;
		for (i = next_pnode - 1; i >= insert_pos; i--) {
			node_index[j] = node_index[i];
			j--;
		}
		node_index[insert_pos] = &(ret->nodes[next_pnode++]);
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

/* Helper: recursive DFS to generate Huffman codes */
static void generate_codes_recursive(node *n, unsigned int current_code, int current_length,
                                     unsigned int codes[256], int code_lengths[256]) {
	if (n == NULL) return;

	/* Leaf node: store the code */
	if (n->byte >= 0) {
		codes[n->byte] = current_code;
		code_lengths[n->byte] = current_length;
		return;
	}

	/* Traverse left: append 0 */
	generate_codes_recursive(n->left, (current_code << 1) | 0, current_length + 1, codes, code_lengths);
	/* Traverse right: append 1 */
	generate_codes_recursive(n->right, (current_code << 1) | 1, current_length + 1, codes, code_lengths);
}

int generate_codes(tree *t, unsigned int codes[256], int code_lengths[256]) {
	if (!t || !t->root) return 0;

	/* Initialize arrays */
	for (int i = 0; i < 256; i++) {
		codes[i] = 0;
		code_lengths[i] = 0;
	}

	/* Handle single-symbol tree (root is a leaf) */
	if (t->root->byte >= 0) {
		codes[t->root->byte] = 0;
		code_lengths[t->root->byte] = 1;
		return 1;
	}
	/* Traverse the tree and assign codes */
	generate_codes_recursive(t->root, 0, 0, codes, code_lengths);

	/* Count symbols with non-zero frequency */
	int num_symbols = 0;
	for (int i = 0; i < 256; i++) {
		if (code_lengths[i] > 0) {
			num_symbols++;
		}
	}
	return num_symbols;
}


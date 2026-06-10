#include <stdio.h>

#include "data_structures/tree.h"
#include "utils/metric.h"
#include "utils/sort.h"

/* Tree creation / destruction */
tree* new_tree(void) {
	tree *ret = (tree*)malloc(sizeof(tree));
	if (ret != NULL) {
		ret->root = NULL;
		/* Initialize all nodes in the array as empty */
		for (int i = 0; i < 512; i++) {
			ret->nodes[i].left = NULL;
			ret->nodes[i].right = NULL;
			ret->nodes[i].byte = -1;
			ret->nodes[i].weight = 0;
		}
	}
	return ret;
}

void delete_tree(tree *del) {
	if (del != NULL) {
		free(del);
	}
}

/* Insert a node into the tree's array and update root if needed */
int tree_insert(tree *t, node *n) {
	if (!t || !n) return -1;

	/* Find empty slot in the nodes array */
	for (int i = 0; i < 512; i++) {
		if (t->nodes[i].byte == -1) {
			/* Copy the node */
			t->nodes[i] = *n;

			/* If this is the root, set it */
			if (!t->root) {
				t->root = &(t->nodes[i]);
			}

			return 0;
		}
	}

	return -1; /* Tree is full */
}

tree *new_tree_from_metric(metric * met) {
	if (!met) return NULL;

	int first_node = 0;
	int last_node = 0;
	int next_pnode = 256;
	int i;

	/* Count distinct symbols with non-zero frequency */
	int num_distinct = 0;
	for (i = 0; i < 256; i++) {
		if (met->characters[i] > 0) {
			num_distinct++;
		}
	}

	/* Check if all frequencies are zero */
	if (num_distinct == 0) {
		/* Create a tree with a single null leaf node */
		tree *ret = new_tree();
		if (!ret) return NULL;
		ret->nodes[0].left = NULL;
		ret->nodes[0].right = NULL;
		ret->nodes[0].byte = -1;
		ret->nodes[0].weight = 0;
		ret->root = &(ret->nodes[0]);
		return ret;
	}

	tree * ret = new_tree();
	if (!ret) return NULL;

	node * node_index[512];

	/* Initialize leaf nodes ONLY for bytes with non-zero frequency */
	for (i = 0; i < 256; i++) {
		if (met->characters[i] > 0) {
			ret->nodes[last_node].left = NULL;
			ret->nodes[last_node].right = NULL;
			ret->nodes[last_node].byte = i;
			ret->nodes[last_node].weight = met->characters[i];
			node_index[last_node] = &(ret->nodes[last_node]);
			last_node++;
		}
	}

	/* Sort the array by weight (ascending) */
	sort_nodes_by_weight(node_index, last_node);

	/* Handle single-symbol case: create a root with one child */
	if (last_node == 1) {
		node *root = &ret->nodes[next_pnode];
		root->left = node_index[0];
		root->right = NULL;
		root->byte = -1;
		root->weight = node_index[0]->weight;
		ret->root = root;
		return ret;
	}

	/* Debug: print initial leaf nodes */
	fprintf(stderr, "DEBUG tree: initial leaves: ");
	for (i = 0; i < last_node; i++) {
		fprintf(stderr, "%c(%llu) ", node_index[i]->byte >= 0 ? (char)node_index[i]->byte : '?',
			(unsigned long long)node_index[i]->weight);
	}
	fprintf(stderr, "\n");

	/* While there are more than 1 nodes in the active set, combine the two smallest.
	 * The active set is node_index[first_node .. last_node-1].
	 * Its size is (last_node - first_node). We need at least 2 elements. */
	while (last_node - first_node > 1) {
		/* Create a parent node from the two smallest (at the front of the active set) */
		ret->nodes[next_pnode].left = node_index[first_node];
		ret->nodes[next_pnode].right = node_index[first_node + 1];
		ret->nodes[next_pnode].weight = node_index[first_node]->weight + node_index[first_node + 1]->weight;
		ret->nodes[next_pnode].byte = -1;
		fprintf(stderr, "DEBUG tree: parent[%d] left=%c(%llu) right=%c(%llu) weight=%llu\n",
			next_pnode,
			node_index[first_node]->byte >= 0 ? (char)node_index[first_node]->byte : '?',
			(unsigned long long)node_index[first_node]->weight,
			node_index[first_node + 1]->byte >= 0 ? (char)node_index[first_node + 1]->byte : '?',
			(unsigned long long)node_index[first_node + 1]->weight,
			(unsigned long long)ret->nodes[next_pnode].weight);
		first_node += 2; /* Consume the two nodes we just used */

		/* Insert the parent node into the sorted position within the active set.
		 * The active set now spans node_index[first_node .. last_node-1],
		 * and we need to insert the new parent while maintaining sort order. */
		int insert_pos = first_node;
		while (insert_pos < last_node && get_weight(node_index[insert_pos]) <= ret->nodes[next_pnode].weight) {
			insert_pos++;
		}

		/* Shift elements to make room for the new parent.
		 * We shift from the end of the active set (last_node - 1) down to insert_pos. */
		for (int k = last_node - 1; k >= insert_pos; k--) {
			node_index[k + 1] = node_index[k];
		}
		node_index[insert_pos] = &(ret->nodes[next_pnode]);
		next_pnode++;
		fprintf(stderr, "DEBUG tree: inserted parent[%d] at pos %d, active set size=%d\n",
			next_pnode - 1, insert_pos, last_node - first_node + 1);
	}

	ret->root = node_index[first_node];
	fprintf(stderr, "DEBUG tree: root=%p byte=%d\n", (void*)ret->root, ret->root->byte);
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
        fprintf(stderr, "DEBUG gen_codes: leaf byte=%c code=%u len=%d\n", (char)n->byte, current_code, current_length);
        codes[n->byte] = current_code;
        code_lengths[n->byte] = current_length;
        return;
    }

    fprintf(stderr, "DEBUG gen_codes: internal byte=%d left=%p right=%p\n", n->byte, (void*)n->left, (void*)n->right);

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
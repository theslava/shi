/* Released to the public domain — No rights reserved. */

#ifndef __tree_h__
#define __tree_h__

#include <malloc.h>

#include "data_structures/node.h"
#include "utils/sort.h"
#include "utils/metric.h"

#define MAX_NODES 512

typedef struct _tree {
    node* root;
    node nodes[MAX_NODES]; /* array of nodes */
} tree;

/* Tree creation / destruction */
tree* new_tree(void);
void delete_tree(tree* del);

/* Insert a node into the tree */
node* insert_sorted(node* head, node* new_node);

tree* new_tree_from_metric(metric* met);

/* Generate Huffman codes from the tree.
 * Fills `codes` (array of 256 unsigned ints) and `code_lengths` (array of 256 ints).
 * Returns number of distinct symbols in the tree. */
int generate_codes(tree* t, unsigned int codes[256], int code_lengths[256]);

/* Free dynamically allocated nodes in the tree (not the static array) */
void free_tree_nodes(node* root);

#endif

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
#include "data_structures/node.h"
#include "utils/sort.h"
#include "utils/metric.h"
typedef struct _tree {
	node * root;
	node nodes[512]; /* array of nodes */
} tree;

/* Tree creation / destruction */
tree* new_tree(void);
void delete_tree(tree *del);

tree *new_tree_from_metric(metric *met);

/* Generate Huffman codes from the tree.
 * Fills `codes` (array of 256 unsigned ints) and `code_lengths` (array of 256 ints).
 * Returns number of distinct symbols in the tree. */
int generate_codes(tree *t, unsigned int codes[256], int code_lengths[256]);

/* Free dynamically allocated nodes in the tree (not the static array) */
void free_tree_nodes(node *root);

#endif


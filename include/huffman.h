#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include "node.h"
#include "metric.h"

typedef struct {
	int *code;
	int length;
} huffman_code;

static inline node * new_tree_node(node *l, node *r) {
	node * ret = (node *)malloc(sizeof(node));
	ret->weight = l->weight + r->weight;
	ret->byte = -1;
	ret->right = r;
	ret->left = l;
	return ret;
}

huffman_code ** make_huffman_codes(int len) {
	huffman_code **codes = (huffman_code **)malloc(len * sizeof(huffman_code *));
	for (int i = 0; i < len; i++) {
		codes[i] = (huffman_code *)malloc(sizeof(huffman_code));
		codes[i]->code = (int *)malloc(len * sizeof(int));
		codes[i]->length = 0;
	}
	return codes;
}

void make_codes(node *root, char *str, int pos, huffman_code **codes) {
	if (!root) return;

	if (!root->left && !root->right) {
		// Leaf node - store the code
		if (pos > 0) {
			// Copy the code
			codes[root->byte]->length = pos;
			for (int i = 0; i < pos; i++) {
				codes[root->byte]->code[i] = str[i] - '0';
			}
		} else {
			// Special case for single character
			codes[root->byte]->length = 1;
			codes[root->byte]->code[0] = 0;
		}
		return;
	}

	str[pos] = '0';
	make_codes(root->left, str, pos + 1, codes);

	str[pos] = '1';
	make_codes(root->right, str, pos + 1, codes);
}

#endif /* HUFFMAN_H */

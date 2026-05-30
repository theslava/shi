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
		// Leaf node
		codes[root->byte]->code[root->byte] = root->byte;
		codes[root->byte]->length = pos;
		return;
	}

	str[pos] = '0';
	make_codes(root->left, str, pos + 1, codes);

	str[pos] = '1';
	make_codes(root->right, str, pos + 1, codes);
}

#endif /* HUFFMAN_H */


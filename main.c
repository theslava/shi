/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/*
 *      main.c
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

/* COMPRESSOR FLOW
 * - open file
 * - build metric out of file
 * - build tree out of metric
 * - compress file with the tree
 * In OO terms:
 * compress_file(new_tree(new_metric(new_file("file_path")))));
 */

#include <stdio.h>
#include <stdlib.h>
#include "file_reader.h"
#include "metric.h"
#include "node.h"
#include "sort.h"
#include "tree.h"
#include "huffman.h"

void compress () {
	fr_fd *file = fr_new("/home/slavik/code/anjuta/shi/data/amrd.bin", 4096);
	metric *met = new_metric_from_file(file);

	// Build metric
	fill_metric(met, file);

	// Create heap
	node *heap[256];
	int heap_size = 0;
	for (int i = 0; i < 256; i++) {
		if (met->characters[i] > 0) {
			node *n = (node *)malloc(sizeof(node));
			n->byte = i;
			n->weight = met->characters[i];
			n->left = NULL;
			n->right = NULL;
			heap[heap_size++] = n;
		}
	}

	if (heap_size == 0) {
	fr_done(file);
	delete_metric(met);
		return;
}

	// Build heap
	for (int i = (heap_size / 2) - 1; i >= 0; i--) {
		headify(heap, heap_size, i);
	}

	// Build tree
	for (int i = 0; i < heap_size - 1; i++) {
		// Extract min
		node *min1 = heap[0];
		headify(heap, heap_size, 0);
		heap_size--;
		swap(&heap[0], &heap[heap_size]);

		node *min2 = heap[0];
		headify(heap, heap_size, 0);
		heap_size--;
		swap(&heap[0], &heap[heap_size]);

		// Create new node
		node *new_node = new_tree_node(min1, min2);
		heap[heap_size] = new_node;
		headify(heap, heap_size, heap_size);
		heap_size++;
}

	node *root = heap[0];

	// Make codes
	char *str = (char *)malloc(256 * sizeof(char));
	huffman_code **codes = make_huffman_codes(256);
	make_codes(root, str, 0, codes);

	// Print codes
	for (int i = 0; i < 256; i++) {
		if (met->characters[i] > 0) {
			printf("Char: %d, Code: ", i);
			for (int j = 0; j < codes[i]->length; j++) {
				printf("%d", codes[i]->code[j]);
			}
			printf("\n");
		}
	}

	free(str);
	for (int i = 0; i < 256; i++) {
		free(codes[i]->code);
		free(codes[i]);
	}
	free(codes);

	fr_done(file);
	delete_metric(met);
	delete_tree(root);
}

void swap(node **a, node **b) {
	node *temp = *a;
	*a = *b;
	*b = temp;
}

void headify(node **heap, int size, int i) {
	node *smallest = heap[i];
	int left = 2 * i + 1;
	int right = 2 * i + 2;

	if (left < size && compare_nodes(heap[left], smallest) < 0) {
		smallest = heap[left];
		i = left;
	}

	if (right < size && compare_nodes(heap[right], smallest) < 0) {
		smallest = heap[right];
		i = right;
	}

	if (i != 2 * i + 1 && i != 2 * i + 2) {
		swap(&heap[i], &heap[2 * i + 1]);
		headify(heap, size, i);
	}
}

int main() {
	compress();
	return 0;
}



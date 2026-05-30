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

void print_help();
void compress(const char *input_file, const char *output_file);
void swap(node **a, node **b);

void print_help() {
	printf("Usage: shi <action> <input_file> [output_file]\n");
	printf("\nActions:\n");
	printf("  compress  Compress the input file using Huffman coding\n");
	printf("\nExamples:\n");
	printf("  shi compress data.bin\n");
	printf("  shi compress data.bin compressed.bin\n");
}

void swap(node **a, node **b) {
	node *temp = *a;
	*a = *b;
	*b = temp;
}

void compress (const char *input_file, const char *output_file) {
	fr_fd *file = fr_new(input_file, 4096);
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
		heapify(heap, heap_size, i);
	}

	// Build tree
	for (int i = 0; i < heap_size - 1; i++) {
		// Extract min
		node *min1 = heap[0];
		heapify(heap, heap_size, 0);
		heap_size--;
		swap(&heap[0], &heap[heap_size]);

		node *min2 = heap[0];
		heapify(heap, heap_size, 0);
		heap_size--;
		swap(&heap[0], &heap[heap_size]);

		// Create new node
		node *new_node = new_tree_node(min1, min2);
		heap[heap_size] = new_node;
		heapify(heap, heap_size, heap_size);
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
	// Free the Huffman tree
	free(root->left);
	free(root->right);
	free(root);
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		print_help();
		return 1;
	}

	const char *action = argv[1];
	const char *input_file = argv[2];
	const char *output_file = (argc >= 4) ? argv[3] : NULL;

	if (strcmp(action, "compress") == 0) {
		compress(input_file, output_file);
	} else {
		fprintf(stderr, "Unknown action: %s\n", action);
		print_help();
		return 1;
	}

	return 0;
}



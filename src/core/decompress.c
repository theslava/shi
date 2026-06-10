/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/*
 *      decompress.c
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

#include <stdlib.h>
#include <stdio.h>

#include "io/file_io.h"
#include "data_structures/node.h"
#include "data_structures/tree.h"
#include "data_structures/bitstream.h"
#include "core/compress.h"
#include "core/decompress.h"

int decompress_file(const char* input_file, const char* output_file) {
	fprintf(stderr, "DEBUG: Opening input file '%s'...\n", input_file);
	fr_fd *input_fd = fr_new((char*)input_file, 4096);
	if (!input_fd) {
		fprintf(stderr, "Error: Could not open input file '%s'\n", input_file);
		return -1;
	}
	fprintf(stderr, "DEBUG: Input file opened.\n");

	/* 2. Read the header to reconstruct codes and code lengths */
	unsigned int codes[256];
	int code_lengths[256];
	fprintf(stderr, "DEBUG: Reading header...\n");
	int num_symbols = read_header(input_fd, codes, code_lengths);
	fprintf(stderr, "DEBUG: Header read, num_symbols=%d\n", num_symbols);
	if (num_symbols < 0) {
		fprintf(stderr, "Error: Could not read header from '%s'\n", input_file);
		fr_done(input_fd);
		return -1;
	}
	/* Handle empty file: no data to decompress */
	if (num_symbols == 0) {
		fw_fd *output_fd = fw_new(output_file, 4096);
		if (!output_fd) {
			fprintf(stderr, "Error: Could not open output file '%s'\n", output_file);
			fr_done(input_fd);
			return -1;
		}
		fw_done(output_fd);
		fr_done(input_fd);
		printf("Decompression complete: '%s' -> '%s' (empty file)\n", input_file, output_file);
		return 0;
	}

	/* 3. Reconstruct the Huffman tree from the codes */
	fprintf(stderr, "DEBUG: Reconstructing tree...\n");
	node *tree_root = reconstruct_tree_from_codes(codes, code_lengths, num_symbols);
	fprintf(stderr, "DEBUG: Tree reconstructed, root=%p\n", (void*)tree_root);
	if (!tree_root) {
		fprintf(stderr, "Error: Could not reconstruct Huffman tree\n");
		fr_done(input_fd);
		return -1;
	}

	/* 4. Open the output file for writing */
	fw_fd *output_fd = fw_new(output_file, 4096);
	if (!output_fd) {
		fprintf(stderr, "Error: Could not open output file '%s'\n", output_file);
		fr_done(input_fd);
		free_tree_nodes(tree_root);
		return -1;
	}

	/* 5. Decompress the data using the reconstructed tree */
	if (decompress_data(input_fd, output_fd, tree_root) != 0) {
		fprintf(stderr, "Error: Decompression failed\n");
		fw_done(output_fd);
		fr_done(input_fd);
		free_tree_nodes(tree_root);
		return -1;
	}

	/* 6. Flush and close the output file */
	fw_flush(output_fd);
	fw_done(output_fd);

	/* 7. Clean up */
	free_tree_nodes(tree_root);
	fr_done(input_fd);

	printf("Decompression complete: '%s' -> '%s'\n", input_file, output_file);
	return 0;
}

int read_header(fr_fd *input_fd, unsigned int codes[256], int code_lengths[256]) {
	fprintf(stderr, "DEBUG read_header: starting\n");
	if (!input_fd || !codes || !code_lengths) { fprintf(stderr, "DEBUG read_header: NULL check failed\n"); return -1; }

	/* Initialize arrays */
	for (int i = 0; i < 256; i++) {
		codes[i] = 0;
		code_lengths[i] = 0;
	}

	/* Read num_symbols as 4-byte little-endian integer */
	fprintf(stderr, "DEBUG read_header: reading 4 bytes for num_symbols\n");
	int b0 = fr_read(input_fd);
	fprintf(stderr, "DEBUG read_header: b0=%d\n", b0);
	if (b0 == EOF) { fprintf(stderr, "DEBUG read_header: EOF on b0\n"); return -1; }
	int b1 = fr_read(input_fd);
	fprintf(stderr, "DEBUG read_header: b1=%d\n", b1);
	if (b1 == EOF) { fprintf(stderr, "DEBUG read_header: EOF on b1\n"); return -1; }
	int b2 = fr_read(input_fd);
	fprintf(stderr, "DEBUG read_header: b2=%d\n", b2);
	if (b2 == EOF) { fprintf(stderr, "DEBUG read_header: EOF on b2\n"); return -1; }
	int b3 = fr_read(input_fd);
	fprintf(stderr, "DEBUG read_header: b3=%d\n", b3);
	if (b3 == EOF) { fprintf(stderr, "DEBUG read_header: EOF on b3\n"); return -1; }

	int num_symbols = (unsigned char)b0 | ((unsigned char)b1 << 8) | ((unsigned char)b2 << 16) | ((unsigned char)b3 << 24);
	if (num_symbols < 0 || num_symbols > 256) return -1;

	/* For each symbol, read byte value and code length */
	for (int i = 0; i < num_symbols; i++) {
		int byte_val = fr_read(input_fd);
		if (byte_val == EOF) return -1;
		int code_len = fr_read(input_fd);
		if (code_len == EOF) return -1;
		codes[(unsigned char)byte_val] = 0;
		code_lengths[(unsigned char)byte_val] = code_len;
	}

	return num_symbols;
}

int decompress_data(fr_fd *input_fd, fw_fd *output_fd, node *tree_root) {
	if (!input_fd || !output_fd || !tree_root) return -1;

	/* Create a bitstream reader */
	bitstream *bs = bs_new(input_fd);
	if (!bs) return -1;
	fprintf(stderr, "DEBUG decompress_data: bitstream created\n");

	/* Handle single-symbol tree (root is a leaf) */
	if (tree_root->byte >= 0) {
		fprintf(stderr, "DEBUG decompress_data: single-symbol tree\n");
		/* Read remaining bytes from input until EOF */
		int byte;
		while ((byte = fr_read(input_fd)) != EOF) {
			fw_write_byte(output_fd, (unsigned char)byte);
		}
		bs_done(bs);
		return 0;
	}

	fprintf(stderr, "DEBUG decompress_data: traversing tree\n");
	/* Traverse the Huffman tree bit-by-bit */
	node *current = tree_root;
	int nodes_traversed = 0;
	while (!bs_eof(bs)) {
		int bit = bs_read_bit(bs);
		if (bit == -1) break; /* EOF */

		if (bit == 0) {
			current = current->left;
		} else {
			current = current->right;
		}

		if (!current) {
			fprintf(stderr, "DEBUG decompress_data: CRASH - current is NULL after bit=%d\n", bit);
			break;
		}

		/* Reached a leaf node: write the decoded byte */
		if (current->byte >= 0) {
			fw_write_byte(output_fd, (unsigned char)current->byte);
			current = tree_root; /* Reset to root */
		}
		nodes_traversed++;
		if (nodes_traversed % 1000 == 0) {
			fprintf(stderr, "DEBUG decompress_data: %d nodes traversed\n", nodes_traversed);
		}
	}

	fprintf(stderr, "DEBUG decompress_data: done, %d nodes traversed\n", nodes_traversed);
	bs_done(bs);
	return 0;
}

/* ==========================================================================
 * Tree reconstruction from Huffman codes (canonical approach)
 * ========================================================================== */

/* Helper: recursive tree builder — inserts leaf at the given bit path. */
static int tree_insert_leaf(node *current, int bit_pos, int bit, int byte_val) {
	if (bit_pos < 0) {
		/* We've consumed all bits — this should be a leaf */
		current->byte = byte_val;
		return 0;
	}
	if (bit == 0) {
		if (!current->left) {
			current->left = new_node(-1, 0);
			if (!current->left) return -1;
		}
		return tree_insert_leaf(current->left, bit_pos - 1, 0, byte_val);
	} else {
		if (!current->right) {
			current->right = new_node(-1, 0);
			if (!current->right) return -1;
		}
		return tree_insert_leaf(current->right, bit_pos - 1, 1, byte_val);
	}
}

/* Build a Huffman tree from code lengths using canonical Huffman code reconstruction.
 * The header only stores (byte_value, code_length) pairs, so we must reconstruct
 * the canonical codes from code lengths before building the tree. */
node* reconstruct_tree_from_codes(const unsigned int codes[256],
                                   const int code_lengths[256],
                                   int num_symbols) {
	if (!codes || !code_lengths || num_symbols <= 0) return NULL;

	/* Step 1: Find the maximum code length */
	int max_len = 0;
	for (int i = 0; i < 256; i++) {
		if (code_lengths[i] > max_len) {
			max_len = code_lengths[i];
		}
	}

	if (max_len == 0) return NULL;

	/* Step 2: Count how many symbols have each code length */
	int *count = (int *)calloc(max_len + 1, sizeof(int));
	if (!count) return NULL;
	for (int i = 0; i < 256; i++) {
		if (code_lengths[i] > 0) {
			count[code_lengths[i]]++;
		}
	}

	/* Step 3: Compute the first code for each length (canonical Huffman) */
	unsigned int *first_code = (unsigned int *)calloc(max_len + 1, sizeof(unsigned int));
	if (!first_code) { free(count); return NULL; }

	unsigned int code = 0;
	for (int len = 1; len <= max_len; len++) {
		first_code[len] = code;
		code = (code + (unsigned int)count[len]) << 1;
	}

	/* Step 4: Assign canonical codes to each symbol */
	unsigned int *symbol_code = (unsigned int *)calloc(256, sizeof(unsigned int));
	if (!symbol_code) { free(count); free(first_code); return NULL; }
	for (int i = 0; i < 256; i++) {
		if (code_lengths[i] > 0) {
			symbol_code[i] = first_code[code_lengths[i]];
			first_code[code_lengths[i]]++;
		}
	}

	/* Debug: print code lengths and reconstructed codes */
	fprintf(stderr, "DEBUG reconstruct: code lengths: ");
	for (int i = 0; i < 256; i++) {
		if (code_lengths[i] > 0) fprintf(stderr, "%c:%d ", (char)i, code_lengths[i]);
	}
	fprintf(stderr, "\n");

	/* Step 5: Build the tree by inserting each symbol's canonical code */
	node *root = new_node(-1, 0);
	if (!root) { free(count); free(first_code); free(symbol_code); return NULL; }

	for (int i = 0; i < 256; i++) {
		if (code_lengths[i] <= 0) continue;
		int len = code_lengths[i];
		node *current = root;
		/* Insert code bits from MSB (bit_pos = len-1) to LSB (bit_pos = 0) */
		for (int bit_pos = len - 1; bit_pos >= 0; bit_pos--) {
			int bit = (symbol_code[i] >> bit_pos) & 1;
			if (bit == 0) {
				if (!current->left) {
					current->left = new_node(-1, 0);
					if (!current->left) {
						free_tree_nodes(root);
						free(count); free(first_code); free(symbol_code);
						return NULL;
					}
				}
				current = current->left;
			} else {
				if (!current->right) {
					current->right = new_node(-1, 0);
					if (!current->right) {
						free_tree_nodes(root);
						free(count); free(first_code); free(symbol_code);
						return NULL;
					}
				}
				current = current->right;
			}
		}
		/* Set the leaf node's byte value */
		if (current->byte < 0) {
			current->byte = i;
		}
	}

	free(count);
	free(first_code);
	free(symbol_code);
	return root;
}

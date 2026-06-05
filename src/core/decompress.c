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

#include "io/file_io.h"
#include "utils/metric.h"
#include "data_structures/node.h"
#include "data_structures/tree.h"
#include "data_structures/bitstream.h"
#include "core/compress.h"
#include "data_structures/bitarray.h"
#include "core/decompress.h"

int decompress_file(const char* input_file, const char* output_file) {
	/* 1. Open the compressed input file for reading */
	fr_fd *input_fd = fr_new((char*)input_file, 4096);
	if (!input_fd) {
		fprintf(stderr, "Error: Could not open input file '%s'\n", input_file);
		return -1;
	}

	/* 2. Read the header to reconstruct codes and code lengths */
	unsigned int codes[256];
	int code_lengths[256];
	int num_symbols = read_header(input_fd, codes, code_lengths);
	if (num_symbols <= 0) {
		fprintf(stderr, "Error: Could not read header from '%s'\n", input_file);
		fr_done(input_fd);
		return -1;
	}

	/* 3. Reconstruct the Huffman tree from the codes */
	node *tree_root = reconstruct_tree_from_codes(codes, code_lengths, num_symbols);
	if (!tree_root) {
		fprintf(stderr, "Error: Could not reconstruct Huffman tree\n");
		fr_done(input_fd);
		return -1;
	}

	/* 4. Open the output file for writing */
	fr_wd *output_fd = fw_new(output_file, 4096);
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
	if (!input_fd || !codes || !code_lengths) return -1;

	/* Initialize arrays */
	for (int i = 0; i < 256; i++) {
		codes[i] = 0;
		code_lengths[i] = 0;
	}

	/* Read num_symbols as 4-byte little-endian integer */
	int b0 = fr_read(input_fd);
	if (b0 == EOF) return -1;
	int b1 = fr_read(input_fd);
	if (b1 == EOF) return -1;
	int b2 = fr_read(input_fd);
	if (b2 == EOF) return -1;
	int b3 = fr_read(input_fd);
	if (b3 == EOF) return -1;

	int num_symbols = (unsigned char)b0 | ((unsigned char)b1 << 8) | ((unsigned char)b2 << 16) | ((unsigned char)b3 << 24);
	if (num_symbols <= 0 || num_symbols > 256) return -1;

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

int decompress_data(fr_fd *input_fd, fr_wd *output_fd, node *tree_root) {
	if (!input_fd || !output_fd || !tree_root) return -1;

	/* Create a bitstream reader */
	bitstream *bs = bs_new(input_fd);
	if (!bs) return -1;

	/* Handle single-symbol tree (root is a leaf) */
	if (tree_root->byte >= 0) {
		/* Read remaining bytes from input until EOF */
		int byte;
		while ((byte = fr_read(input_fd)) != EOF) {
			fw_write_byte(output_fd, (unsigned char)byte);
		}
		bs_done(bs);
		return 0;
	}

	/* Traverse the Huffman tree bit-by-bit */
	node *current = tree_root;
	while (!bs_eof(bs)) {
		int bit = bs_read_bit(bs);
		if (bit == -1) break; /* EOF */

		if (bit == 0) {
			current = current->left;
		} else {
			current = current->right;
		}

		/* Reached a leaf node: write the decoded byte */
		if (current->byte >= 0) {
			fw_write_byte(output_fd, (unsigned char)current->byte);
			current = tree_root; /* Reset to root */
		}
	}

	bs_done(bs);
	return 0;
}

/* ==========================================================================
 * Tree reconstruction from Huffman codes (canonical approach)
 * ========================================================================== */

/* Build a Huffman tree from code lengths using a prefix-code reconstruction algorithm. */
node* reconstruct_tree_from_codes(const unsigned int codes[256],
                                   const int code_lengths[256],
                                   int num_symbols) {
	if (!codes || !code_lengths || num_symbols <= 0) return NULL;

	/* Find the maximum code length */
	int max_len = 0;
	for (int i = 0; i < 256; i++) {
		if (code_lengths[i] > max_len) {
			max_len = code_lengths[i];
		}
	}

	if (max_len == 0) return NULL;

	/* Handle single-symbol case */
	if (max_len == 1) {
		for (int i = 0; i < 256; i++) {
			if (code_lengths[i] == 1) {
				node *leaf = new_node(i, 0);
				if (!leaf) return NULL;
				node *root = new_node(-1, 0);
				if (!root) { free(leaf); return NULL; }
				root->left = leaf;
				return root;
			}
		}
		return NULL;
	}

	/* Build the tree using the codes directly.
	 * For each symbol, insert its code bits into the tree. */
	node *root = new_node(-1, 0);
	if (!root) return NULL;

	for (int i = 0; i < 256; i++) {
		if (code_lengths[i] <= 0) continue;

		/* Insert this symbol's code into the tree */
		node *current = root;
		for (int bit_pos = code_lengths[i] - 1; bit_pos >= 0; bit_pos--) {
			int bit = (codes[i] >> bit_pos) & 1;
			if (bit == 0) {
				if (!current->left) {
					current->left = new_node(-1, 0);
					if (!current->left) {
						free_tree_nodes(root);
						return NULL;
					}
				}
				current = current->left;
			} else {
				if (!current->right) {
					current->right = new_node(-1, 0);
					if (!current->right) {
						free_tree_nodes(root);
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

	return root;
}

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

#include "file_reader.h"
#include "file_writer.h"
#include "metric.h"
#include "node.h"
#include "tree.h"
#include "bitstream.h"
#include "compress.h"
#include "decompress.h"

void decompress_file(const char* input_file, const char* output_file) {
	/* 1. Open the compressed input file for reading */
	fr_fd *input_fd = fr_new((char*)input_file, 4096);
	if (!input_fd) {
		fprintf(stderr, "Error: Could not open input file '%s'\n", input_file);
		return;
	}

	/* 2. Read the header to reconstruct codes and code lengths */
	unsigned int codes[256];
	int code_lengths[256];
	int num_symbols = read_header(input_fd, codes, code_lengths);
	if (num_symbols <= 0) {
		fprintf(stderr, "Error: Could not read header from '%s'\n", input_file);
		fr_done(input_fd);
		return;
	}

	/* 3. Reconstruct the Huffman tree from the codes */
	node *tree_root = reconstruct_tree_from_codes(codes, code_lengths, num_symbols);
	if (!tree_root) {
		fprintf(stderr, "Error: Could not reconstruct Huffman tree\n");
		fr_done(input_fd);
		return;
	}

	/* 4. Open the output file for writing */
	fr_wd *output_fd = fw_new(output_file, 4096);
	if (!output_fd) {
		fprintf(stderr, "Error: Could not open output file '%s'\n", output_file);
		fr_done(input_fd);
		return;
	}

	/* 5. Decompress the data using the reconstructed tree */
	if (decompress_data(input_fd, output_fd, tree_root) != 0) {
		fprintf(stderr, "Error: Decompression failed\n");
		fw_done(output_fd);
		fr_done(input_fd);
		return;
	}

	/* 6. Flush and close the output file */
	fw_flush(output_fd);
	fw_done(output_fd);

	/* 7. Clean up */
	free_tree_nodes(tree_root);
	fr_done(input_fd);

	printf("Decompression complete: '%s' -> '%s'\n", input_file, output_file);
}

int read_header(fr_fd *input_fd, unsigned int codes[256], int code_lengths[256]) {
	/* TODO: Implement header reading */
	/* Read num_symbols as 4-byte integer (little-endian) */
	/* For each symbol, read byte value and code length */
	/* Fill in the codes[] and code_lengths[] arrays */
	return -1;
}

int decompress_data(fr_fd *input_fd, fr_wd *output_fd, node *tree_root) {
	/* TODO: Implement data decompression */
	/* Create a bitstream reader from input_fd */
	/* Traverse the Huffman tree for each decoded symbol */
	/* Write each decoded byte to output_fd via fw_write_byte() */
	return 0;
}

node* reconstruct_tree_from_codes(const unsigned int codes[256],
                                   const int code_lengths[256],
                                   int num_symbols) {
	/* TODO: Implement tree reconstruction from Huffman codes */
	/* Build a canonical Huffman tree from the code lengths */
	return NULL;
}

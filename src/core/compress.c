/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/*
 *      compress.c
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

int compress_file(const char* input_file, const char* output_file) {
	/* 1. Open the input file for reading */
	fr_fd *input_fd = fr_new((char*)input_file, 4096);
	if (!input_fd) {
		fprintf(stderr, "Error: Could not open input file '%s'\n", input_file);
		return -1;
	}

	/* 2. Build the frequency metric from the input data */
	metric *met = new_metric_from_file(input_fd);
	if (!met) {
		fprintf(stderr, "Error: Could not build metric from '%s'\n", input_file);
		fr_done(input_fd);
		return -1;
	}

	/* 3. Build the Huffman tree from the frequency metric */
	tree *t = new_tree_from_metric(met);
	if (!t || !t->root) {
		fprintf(stderr, "Error: Could not build Huffman tree\n");
		delete_metric(met);
		fr_done(input_fd);
		return -1;
	}

	/* 4. Generate Huffman codes from the tree */
	unsigned int codes[256];
	int code_lengths[256];
	int num_symbols = generate_codes(t, codes, code_lengths);
	if (num_symbols <= 0) {
		fprintf(stderr, "Error: No symbols found in input\n");
		delete_tree(t);
		delete_metric(met);
		fr_done(input_fd);
		return -1;
	}

	/* 5. Open the output file for writing */
	fr_wd *output_fd = fw_new(output_file, 4096);
	if (!output_fd) {
		fprintf(stderr, "Error: Could not open output file '%s'\n", output_file);
		delete_tree(t);
		delete_metric(met);
		fr_done(input_fd);
		return -1;
	}

	/* 6. Write the header (metadata needed for decompression) */
	if (write_header(output_fd, codes, code_lengths, num_symbols) != 0) {
		fprintf(stderr, "Error: Could not write header to '%s'\n", output_file);
		fw_done(output_fd);
		delete_tree(t);
		delete_metric(met);
		fr_done(input_fd);
		return -1;
	}

	/* 7. Compress the data using the generated codes */
	if (compress_data(input_fd, output_fd, codes, code_lengths) != 0) {
		fprintf(stderr, "Error: Compression failed\n");
		fw_done(output_fd);
		delete_tree(t);
		delete_metric(met);
		fr_done(input_fd);
		return -1;
	}

	/* 8. Flush and close the output file */
	fw_flush(output_fd);
	fw_done(output_fd);

	/* 9. Clean up */
	delete_tree(t);
	delete_metric(met);
	fr_done(input_fd);

	printf("Compression complete: '%s' -> '%s'\n", input_file, output_file);
	return 0;
}

int write_header(fr_wd *output_fd, const unsigned int codes[256],
                 const int code_lengths[256], int num_symbols) {
	if (!output_fd || !codes || !code_lengths || num_symbols <= 0) return -1;

	/* Write num_symbols as 4-byte little-endian integer */
	unsigned char header[4];
	header[0] = (unsigned char)(num_symbols & 0xFF);
	header[1] = (unsigned char)((num_symbols >> 8) & 0xFF);
	header[2] = (unsigned char)((num_symbols >> 16) & 0xFF);
	header[3] = (unsigned char)((num_symbols >> 24) & 0xFF);
	if (fw_write_bytes(output_fd, header, 4) != 0) {
		return -1;
	}

	/* For each symbol with non-zero code length, write byte value + code length */
	unsigned char symbol_data[2];
	int count = 0;
	for (int i = 0; i < 256 && count < num_symbols; i++) {
		if (code_lengths[i] > 0) {
			symbol_data[0] = (unsigned char)i;
			symbol_data[1] = (unsigned char)code_lengths[i];
			if (fw_write_bytes(output_fd, symbol_data, 2) != 0) {
				return -1;
			}
			count++;
		}
	}
	return 0;
}

int compress_data(fr_fd *input_fd, fr_wd *output_fd,
                  const unsigned int codes[256], const int code_lengths[256]) {
	if (!input_fd || !output_fd || !codes || !code_lengths) return -1;

	/* Create a bitstream writer */
	bitstream_writer *bsw = bsw_new(output_fd);
	if (!bsw) return -1;

	/* Read each byte from input and write its Huffman code */
	int byte;
	while ((byte = fr_read(input_fd)) != EOF) {
		unsigned char c = (unsigned char)byte;
		if (code_lengths[c] > 0) {
			bsw_write_bits(bsw, codes[c], code_lengths[c]);
		}
	}

	/* Flush remaining bits */
	bsw_flush(bsw);
	bsw_done(bsw);
	return 0;
}

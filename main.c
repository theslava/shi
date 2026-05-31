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
#include <string.h>
#include "file_reader.h"
#include "metric.h"
#include "node.h"
#include "sort.h"
#include "tree.h"

// Function prototypes
void compress_file(const char* input_file, const char* output_file);
void decompress_file(const char* input_file, const char* output_file);

void compress_file(const char* input_file, const char* output_file) {
	fr_fd *file = fr_new(input_file, 4096);
	metric *met = new_metric_from_file(file);
	tree* root = new_tree_from_metric(met);

	// TODO: Implement actual compression logic
	// This would typically involve:
	// - Building a dictionary from the tree
	// - Compressing the file with the dictionary
	fr_done(file);
	delete_metric(met);
	delete_tree(root);
}

void decompress_file(const char* input_file, const char* output_file) {
	// TODO: Implement actual decompression logic
	// This would typically involve:
	// - Reading the compressed file
	// - Reconstructing the tree
	// - Decompressing using the tree
}

int main(int argc, char* argv[]) {
    // Check if we have the right number of arguments
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <compress|decompress> <input_file> <output_file>\n", argv[0]);
        fprintf(stderr, "Example: %s compress input.txt output.txt\n", argv[0]);
        fprintf(stderr, "Example: %s decompress input.txt output.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Get the command
    const char* command = argv[1];
    const char* input_file = argv[2];
    const char* output_file = argv[3];

    // Check which command was given
    if (strcmp(command, "compress") == 0) {
        printf("Compressing '%s' to '%s'\n", input_file, output_file);
        compress_file(input_file, output_file);
        return EXIT_SUCCESS;
    }
    else if (strcmp(command, "decompress") == 0) {
        printf("Decompressing '%s' to '%s'\n", input_file, output_file);
        decompress_file(input_file, output_file);
        return EXIT_SUCCESS;
    }
    else {
        fprintf(stderr, "Error: Unknown command '%s'. Use 'compress' or 'decompress'.\n", command);
        return EXIT_FAILURE;
    }
}


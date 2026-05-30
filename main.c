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
void headify(node **heap, int size, int i);

void print_help() {
	printf("Usage: shi <action> <input_file> [output_file]\n");
	printf("\nActions:\n");
	printf("  compress  Compress the input file using Huffman coding\n");
	printf("\nExamples:\n");
	printf("  shi compress data.bin\n");
	printf("  shi compress data.bin compressed.bin\n");
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


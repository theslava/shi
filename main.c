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

void compress () {
	fr_fd *file = fr_new("/home/slavik/code/anjuta/shi/data/amrd.bin",4096);
	metric *met = new_metric_from_file(file);
	tree* root = new_tree_from_metric(met);
	//build_dictionary_from_tree(root)
	//compress_file(file, output_file, dict)
	fr_done(file);
	delete_metric(met);
	delete_tree(root);
}

int main() {
	compress();
	return 0;
}

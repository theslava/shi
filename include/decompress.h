/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

#ifndef __decompress_h__
#define __decompress_h__

#include "file_reader.h"
#include "tree.h"

/* High-level entry point (existing) */
void decompress_file(const char* input_file, const char* output_file);

/* --- Internal helpers (new stubs) --- */

/* Reconstruct a Huffman tree from codes and code_lengths read from the header.
 * Returns pointer to root node, or NULL on error. */
node* reconstruct_tree_from_codes(const unsigned int codes[256],
                                   const int code_lengths[256],
                                   int num_symbols);

#endif
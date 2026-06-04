/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

#ifndef __compress_h__
#define __compress_h__

#include "utils/metric.h"
#include "data_structures/tree.h"
#include "io/file_reader.h"

/* High-level entry point (existing) */
void compress_file(const char* input_file, const char* output_file);

/* --- Internal helpers (new stubs) --- */

/* Write the Huffman tree metadata to the output file so it can be reconstructed.
 * Writes: number of symbols, then for each symbol: byte value + code length. */
int write_header(fr_wd *output_fd, const unsigned int codes[256],
                 const int code_lengths[256], int num_symbols);

/* Read the Huffman tree metadata from the input file and reconstruct `codes` / `code_lengths`.
 * Returns number of symbols read, or -1 on error. */
int read_header(fr_fd *input_fd, unsigned int codes[256], int code_lengths[256]);

/* Compress the input data using the Huffman codes and write to output via bitstream writer.
 * `codes` and `code_lengths` are arrays of size 256 indexed by byte value. */
int compress_data(fr_fd *input_fd, fr_wd *output_fd,
                  const unsigned int codes[256], const int code_lengths[256]);

/* Decompress data from input bitstream using the Huffman tree and write to output file.
 * Returns 0 on success, -1 on error. */
int decompress_data(fr_fd *input_fd, fr_wd *output_fd, node *tree_root);

#endif

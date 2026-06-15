/* Released to the public domain — No rights reserved. */

#include <stdio.h>
#include <stddef.h>

#include "io/file_io.h"
#include "data_structures/node.h"
#include "data_structures/tree.h"
#include "data_structures/bitstream.h"
#include "core/decompress.h"
#include "core/compress.h"
#include "core/version.h"

/* ==========================================================================
 * High-level decompression entry point
 * ========================================================================== */

int decompress_file(const char* input_file, const char* output_file) {
    /* 1. Open the compressed input file */
    fr_fd* input_fd = fr_new((char*)input_file, 4096);
    if (!input_fd) {
        fprintf(stderr, "Error: Could not open input file '%s'\n", input_file);
        return -1;
    }

    /* 2. Verify magic bytes */
    unsigned char magic[4];
    for (int i = 0; i < 4; i++) {
        int b = fr_read(input_fd);
        if (b == EOF) {
            fprintf(stderr, "Error: Could not read magic bytes from '%s'\n", input_file);
            fr_done(input_fd);
            return -1;
        }
        magic[i] = (unsigned char)b;
    }
    if (magic[0] != 0x53 || magic[1] != 0x48 || magic[2] != 0x49 || magic[3] != 0x00) {
        fprintf(stderr, "Error: Invalid file format - bad magic bytes in '%s'\n", input_file);
        fr_done(input_fd);
        return -1;
    }

    /* 3. Read the header to get codes, code_lengths, and file_size */
    unsigned int codes[256];
    int code_lengths[256];
    unsigned int file_size = 0;
    int num_symbols = read_header(input_fd, codes, code_lengths, &file_size);
    if (num_symbols < 0) {
        fprintf(stderr, "Error: Could not read header from '%s'\n", input_file);
        fr_done(input_fd);
        return -1;
    }

    /* Handle empty file: no data to decompress */
    if (num_symbols == 0) {
        /* Create empty output file */
        fw_fd* output_fd = fw_new(output_file, 4096);
        if (!output_fd) {
            fprintf(stderr, "Error: Could not create output file '%s'\n", output_file);
            fr_done(input_fd);
            return -1;
        }
        fw_done(output_fd);
        fr_done(input_fd);
        printf("Decompression complete: '%s' -> '%s' (empty file)\n", input_file, output_file);
        return 0;
    }

    /* 4. Reconstruct the Huffman tree from the codes */
    node* root = reconstruct_tree_from_codes(codes, code_lengths, num_symbols);
    if (!root) {
        fprintf(stderr, "Error: Could not reconstruct Huffman tree\n");
        fr_done(input_fd);
        return -1;
    }

    /* 5. Open the output file for writing */
    fw_fd* output_fd = fw_new(output_file, 4096);
    if (!output_fd) {
        fprintf(stderr, "Error: Could not create output file '%s'\n", output_file);
        free_tree_nodes(root);
        fr_done(input_fd);
        return -1;
    }

    /* 6. Decompress the data */
    if (decompress_data(input_fd, output_fd, root, file_size) != 0) {
        fprintf(stderr, "Error: Decompression failed\n");
        fw_done(output_fd);
        free_tree_nodes(root);
        fr_done(input_fd);
        return -1;
    }

    /* 7. Flush and close the output file */
    fw_flush(output_fd);
    fw_done(output_fd);

    /* 8. Clean up */
    free_tree_nodes(root);
    fr_done(input_fd);

    printf("Decompression complete: '%s' -> '%s'\n", input_file, output_file);
    return 0;
}

/* ==========================================================================
 * Header reading
 * ========================================================================== */

int read_header(fr_fd* input_fd,
                unsigned int codes[256],
                int code_lengths[256],
                unsigned int* file_size_out) {
    if (!input_fd || !codes || !code_lengths)
        return -1;

    /* Initialize arrays */
    for (int i = 0; i < 256; i++) {
        codes[i] = 0;
        code_lengths[i] = 0;
    }

    /* Read num_symbols as 4-byte little-endian integer */
    unsigned char header[4];
    for (int i = 0; i < 4; i++) {
        int b = fr_read(input_fd);
        if (b == EOF)
            return -1;
        header[i] = (unsigned char)b;
    }

    int num_symbols = header[0] | (header[1] << 8) | (header[2] << 16) | (header[3] << 24);

    /* Read original file size as 4-byte little-endian integer */
    unsigned char size_bytes[4];
    for (int i = 0; i < 4; i++) {
        int b = fr_read(input_fd);
        if (b == EOF)
            return -1;
        size_bytes[i] = (unsigned char)b;
    }
    unsigned int file_size =
        size_bytes[0] | (size_bytes[1] << 8) | (size_bytes[2] << 16) | (size_bytes[3] << 24);

    if (num_symbols < 0 || num_symbols > 256)
        return -1;

    /* Store file_size output */
    if (file_size_out) {
        *file_size_out = file_size;
    }

    /* For each symbol, read byte_value (1B) + code_length (1B) + code_value (4B LE) */
    unsigned char code_bytes[4];

    for (int i = 0; i < num_symbols; i++) {
        /* Read byte_value */
        int b1 = fr_read(input_fd);
        int b2 = fr_read(input_fd);
        if (b1 == EOF || b2 == EOF)
            return -1;
        unsigned char byte_value = (unsigned char)b1;
        unsigned char code_length = (unsigned char)b2;

        /* Read code_value (4-byte LE) */
        int c1 = fr_read(input_fd);
        int c2 = fr_read(input_fd);
        int c3 = fr_read(input_fd);
        int c4 = fr_read(input_fd);
        if (c1 == EOF || c2 == EOF || c3 == EOF || c4 == EOF)
            return -1;
        code_bytes[0] = (unsigned char)c1;
        code_bytes[1] = (unsigned char)c2;
        code_bytes[2] = (unsigned char)c3;
        code_bytes[3] = (unsigned char)c4;

        unsigned int code_value =
            code_bytes[0] | (code_bytes[1] << 8) | (code_bytes[2] << 16) | (code_bytes[3] << 24);

        codes[byte_value] = code_value;
        code_lengths[byte_value] = code_length;
    }

    return num_symbols;
}

/* ==========================================================================
 * Tree reconstruction from actual codes
 * ========================================================================== */

node* reconstruct_tree_from_codes(const unsigned int codes[256],
                                  const int code_lengths[256],
                                  int num_symbols) {
    if (!codes || !code_lengths || num_symbols <= 0)
        return NULL;

    /* Build the tree by inserting each symbol's actual code */
    node* root = new_node(-1, 0);
    if (!root)
        return NULL;

    for (int i = 0; i < 256; i++) {
        if (code_lengths[i] <= 0)
            continue;
        int len = code_lengths[i];
        unsigned int code = codes[i];
        node* current = root;
        /* Insert code bits from MSB (bit_pos = len-1) to LSB (bit_pos = 0) */
        for (int bit_pos = len - 1; bit_pos >= 0; bit_pos--) {
            int bit = (code >> bit_pos) & 1;
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

/* ==========================================================================
 * Data decompression
 * ========================================================================== */

int decompress_data(fr_fd* input_fd, fw_fd* output_fd, node* tree_root, unsigned int file_size) {
    if (!input_fd || !output_fd || !tree_root)
        return -1;

    /* Create a bitstream reader */
    bitstream* bs = bs_new(input_fd);
    if (!bs)
        return -1;

    /* Traverse the tree bit-by-bit to decode symbols */
    node* current = tree_root;
    unsigned int byte_count = 0;

    while (byte_count < file_size) {
        int bit = bs_read_bit(bs);
        if (bit == -1) {
            /* EOF reached before expected file_size */
            break;
        }

        if (bit == 0) {
            current = current->left;
        } else {
            current = current->right;
        }

        /* If we reached a leaf node, output the byte and reset to root */
        if (current->byte >= 0) {
            if (fw_write_byte(output_fd, (unsigned char)current->byte) != 0) {
                bs_done(bs);
                return -1;
            }
            byte_count++;
            current = tree_root;
        }
    }

    /* Flush the bitstream */
    bs_done(bs);

    return 0;
}

/* ==========================================================================
 * Version-specific decompression entry point (v0)
 * ========================================================================== */

int shi_decompress_v0(const char* input_file, const char* output_file) {
    return decompress_file(input_file, output_file);
}

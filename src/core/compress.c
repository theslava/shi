/* Released to the public domain — No rights reserved. */

#include <stdio.h>

#include "io/file_io.h"
#include "utils/metric.h"
#include "data_structures/node.h"
#include "data_structures/tree.h"
#include "data_structures/bitstream.h"
#include "core/compress.h"
#include "core/version.h"

#define MAGIC_BYTES "\x53\x48\x49\x00"

/* Magic bytes for version 0 (SHI\x00) */
const unsigned char shi_magic_v0[4] = {0x53, 0x48, 0x49, 0x00};

static int verbose = 0;

void set_verbose(int v) {
    verbose = v;
}

int compress_file(const char* input_file, const char* output_file) {
    /* 1. Open the input file for reading */
    fr_fd* input_fd = fr_new((char*)input_file, 4096);
    if (!input_fd) {
        fprintf(stderr, "Error: Could not open input file '%s'\n", input_file);
        return -1;
    }

    /* 2. Build the frequency metric from the input data (also returns file_size) */
    unsigned int file_size = 0;
    metric* met = new_metric_from_file(input_fd, &file_size);
    if (!met) {
        fprintf(stderr, "Error: Could not build metric from '%s'\n", input_file);
        fr_done(input_fd);
        return -1;
    }

    /* 3. Build the Huffman tree from the frequency metric */
    tree* t = new_tree_from_metric(met);
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
    if (num_symbols < 0) {
        fprintf(stderr, "Error: Could not generate codes\n");
        delete_tree(t);
        delete_metric(met);
        fr_done(input_fd);
        return -1;
    }

    /* 5. Open the output file for writing */
    fw_fd* output_fd = fw_new(output_file, 4096);
    if (!output_fd) {
        fprintf(stderr, "Error: Could not open output file '%s'\n", output_file);
        delete_tree(t);
        delete_metric(met);
        fr_done(input_fd);
        return -1;
    }

    /* 6. Write magic bytes "SHI\x00" */
    const unsigned char magic[4] = {0x53, 0x48, 0x49, 0x00};
    (void)MAGIC_BYTES; /* kept for explicit byte values; define kept for reference */
    if (fw_write_bytes(output_fd, magic, 4) != 0) {
        fprintf(stderr, "Error: Could not write magic bytes to '%s'\n", output_file);
        fw_done(output_fd);
        delete_tree(t);
        delete_metric(met);
        fr_done(input_fd);
        return -1;
    }

    /* 8. Write the header (metadata needed for decompression) */
    if (write_header(output_fd, codes, code_lengths, num_symbols, file_size) != 0) {
        fprintf(stderr, "Error: Could not write header to '%s'\n", output_file);
        fw_done(output_fd);
        delete_tree(t);
        delete_metric(met);
        fr_done(input_fd);
        return -1;
    }

    /* Handle empty file: no data to compress */
    if (num_symbols == 0) {
        fw_flush(output_fd);
        fw_done(output_fd);
        delete_tree(t);
        delete_metric(met);
        fr_done(input_fd);
        printf("Compression complete: '%s' -> '%s' (empty file)\n", input_file, output_file);
        return 0;
    }

    /* 7. Rewind the input file before compressing */
    fr_rewind(input_fd);

    /* 8. Compress the data using the generated codes */
    if (compress_data(input_fd, output_fd, codes, code_lengths) != 0) {
        fprintf(stderr, "Error: Compression failed\n");
        fw_done(output_fd);
        delete_tree(t);
        delete_metric(met);
        fr_done(input_fd);
        return -1;
    }

    /* 9. Flush and close the output file */
    fw_flush(output_fd);
    fw_done(output_fd);

    /* 10. Clean up */
    delete_tree(t);
    delete_metric(met);
    fr_done(input_fd);

    if (verbose) {
        printf("Compression complete: '%s' -> '%s' (%d symbols, file_size=%u)\n", input_file,
               output_file, num_symbols, file_size);
    } else {
        printf("Compression complete: '%s' -> '%s'\n", input_file, output_file);
    }
    return 0;
}

int write_header(fw_fd* output_fd,
                 const unsigned int codes[256],
                 const int code_lengths[256],
                 int num_symbols,
                 unsigned int file_size) {
    if (!output_fd || !codes || !code_lengths)
        return -1;
    if (num_symbols < 0)
        return -1;

    /* Write num_symbols as 4-byte little-endian integer */
    unsigned char header[4];
    header[0] = (unsigned char)(num_symbols & 0xFF);
    header[1] = (unsigned char)((num_symbols >> 8) & 0xFF);
    header[2] = (unsigned char)((num_symbols >> 16) & 0xFF);
    header[3] = (unsigned char)((num_symbols >> 24) & 0xFF);
    if (fw_write_bytes(output_fd, header, 4) != 0) {
        return -1;
    }

    /* Write original file size as 4-byte little-endian integer */
    unsigned char size_bytes[4];
    size_bytes[0] = (unsigned char)(file_size & 0xFF);
    size_bytes[1] = (unsigned char)((file_size >> 8) & 0xFF);
    size_bytes[2] = (unsigned char)((file_size >> 16) & 0xFF);
    size_bytes[3] = (unsigned char)((file_size >> 24) & 0xFF);
    if (fw_write_bytes(output_fd, size_bytes, 4) != 0) {
        return -1;
    }

    /* For each symbol with non-zero code length, write byte value + code length + code value */
    unsigned char symbol_data[2];
    unsigned char code_bytes[4];
    int count = 0;
    for (int i = 0; i < 256 && count < num_symbols; i++) {
        if (code_lengths[i] > 0) {
            symbol_data[0] = (unsigned char)i;
            symbol_data[1] = (unsigned char)code_lengths[i];
            if (fw_write_bytes(output_fd, symbol_data, 2) != 0) {
                return -1;
            }
            /* Write the actual code value as 4-byte LE */
            code_bytes[0] = (unsigned char)(codes[i] & 0xFF);
            code_bytes[1] = (unsigned char)((codes[i] >> 8) & 0xFF);
            code_bytes[2] = (unsigned char)((codes[i] >> 16) & 0xFF);
            code_bytes[3] = (unsigned char)((codes[i] >> 24) & 0xFF);
            if (fw_write_bytes(output_fd, code_bytes, 4) != 0) {
                return -1;
            }
            count++;
        }
    }
    return 0;
}

int compress_data(fr_fd* input_fd,
                  fw_fd* output_fd,
                  const unsigned int codes[256],
                  const int code_lengths[256]) {
    if (!input_fd || !output_fd || !codes || !code_lengths)
        return -1;

    /* Create a bitstream writer */
    bitstream_writer* bsw = bsw_new(output_fd);
    if (!bsw)
        return -1;

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

/* ==========================================================================
 * Version-specific compression entry point (v0)
 * ========================================================================== */

int shi_compress_v0(const char* input_file, const char* output_file) {
    return compress_file(input_file, output_file);
}

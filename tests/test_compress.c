/*
 *      test_compress.c
 *
 *      Tests for the compression API.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_helpers.h"
#include "core/compress.h"
#include "core/decompress.h"

/* Test: Compress and decompress a simple string */
static int test_compress_roundtrip(void) {
    TEST_START("compress -> decompress roundtrip");

    const char* input = "test_compress_input.txt";
    const char* compressed = "test_compress_output.txt";
    const char* decompressed = "test_compress_restored.txt";

    /* Create input file */
    create_temp_file(input, "Hello, world! This is a test for compression.");

    /* Compress */
    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    /* Decompress */
    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompression succeeds");

    /* Compare files */
    TEST_ASSERT(files_equal(input, decompressed) == 1, "decompressed matches original");

    /* Cleanup */
    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* Test: Compress empty file */
static int test_compress_empty(void) {
    TEST_START("compress empty file");

    const char* input = "test_empty.txt";
    const char* compressed = "test_empty_compressed.txt";
    const char* decompressed = "test_empty_decompressed.txt";

    create_temp_file(input, "");

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compress empty file succeeds");

    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompress empty file succeeds");

    TEST_ASSERT(files_equal(input, decompressed) == 1, "empty file roundtrip");

    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* Test: Compress repeated content (highly compressible) */
static int test_compress_repeated(void) {
    TEST_START("compress repeated content");

    const char* input = "test_repeated.txt";
    const char* compressed = "test_repeated_compressed.txt";
    const char* decompressed = "test_repeated_decompressed.txt";

    /* Create a highly repetitive file */
    FILE* fp = fopen(input, "wb");
    if (fp) {
        for (int i = 0; i < 100; i++) {
            fprintf(fp, "AAAA");
        }
        fclose(fp);
    }
    TEST_ASSERT(fp != NULL, "created repeated content file");

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compress repeated content succeeds");

    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompress repeated content succeeds");

    TEST_ASSERT(files_equal(input, decompressed) == 1, "repeated content roundtrip");

    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* Test: Compress single-byte file (exactly one byte) */
static int test_compress_single_byte(void) {
    TEST_START("compress single-byte file");

    const char* input = "test_single_byte.txt";
    const char* compressed = "test_single_byte_compressed.txt";
    const char* decompressed = "test_single_byte_decompressed.txt";

    /* Create a file with exactly one byte */
    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created single-byte file");
    unsigned char byte_val = 0x42;
    fwrite(&byte_val, 1, 1, fp);
    fclose(fp);

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compress single-byte file succeeds");

    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompress single-byte file succeeds");

    TEST_ASSERT(files_equal(input, decompressed) == 1, "single-byte roundtrip");

    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* Test: Compress file with only one distinct symbol */
static int test_compress_single_symbol(void) {
    TEST_START("compress single-symbol file");

    const char* input = "test_single_symbol.txt";
    const char* compressed = "test_single_symbol_compressed.txt";
    const char* decompressed = "test_single_symbol_decompressed.txt";

    /* Create a file with only one distinct symbol repeated */
    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created single-symbol file");
    for (int i = 0; i < 50; i++) {
        fputc('X', fp);
    }
    fclose(fp);

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compress single-symbol file succeeds");

    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompress single-symbol file succeeds");

    TEST_ASSERT(files_equal(input, decompressed) == 1, "single-symbol roundtrip");

    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* Test: Compress binary data with all byte values */
static int test_compress_binary(void) {
    TEST_START("compress binary data (all byte values)");

    const char* input = "test_binary.bin";
    const char* compressed = "test_binary_compressed.bin";
    const char* decompressed = "test_binary_decompressed.bin";

    /* Create a file with all 256 byte values repeated */
    unsigned char data[256];
    for (int i = 0; i < 256; i++) {
        data[i] = (unsigned char)i;
    }

    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created binary file");
    for (int i = 0; i < 4; i++) {
        fwrite(data, 1, 256, fp);
    }
    fclose(fp);

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compress binary data succeeds");

    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompress binary data succeeds");

    TEST_ASSERT(files_equal(input, decompressed) == 1, "binary data roundtrip");

    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* Test: Compress with single-byte file containing 0x00 */
static int test_compress_single_byte_null(void) {
    TEST_START("compress single-byte file (0x00)");

    const char* input = "test_single_null.txt";
    const char* compressed = "test_single_null_compressed.txt";
    const char* decompressed = "test_single_null_decompressed.txt";

    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created null-byte file");
    unsigned char byte_val = 0x00;
    fwrite(&byte_val, 1, 1, fp);
    fclose(fp);

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compress null-byte file succeeds");

    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompress null-byte file succeeds");

    TEST_ASSERT(files_equal(input, decompressed) == 1, "null-byte roundtrip");

    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Header Validation Tests
 * ========================================================================== */

/* Helper: write a valid minimal compressed file (1 symbol, 1 byte 'A') */
static int write_minimal_compressed(const char* path) {
    FILE* fp = fopen(path, "wb");
    TEST_ASSERT(fp != NULL, "created minimal compressed file");

    /* Magic: "SHI\x00" */
    unsigned char magic[4] = {0x53, 0x48, 0x49, 0x00};
    fwrite(magic, 1, 4, fp);

    /* num_symbols = 1 (LE) */
    unsigned char num_sym[4] = {1, 0, 0, 0};
    fwrite(num_sym, 1, 4, fp);

    /* file_size = 1 (LE) */
    unsigned char fsize[4] = {1, 0, 0, 0};
    fwrite(fsize, 1, 4, fp);

    /* Symbol: byte_value=65 ('A'), code_length=1, code_value=0 */
    unsigned char sym[6] = {65, 1, 0, 0, 0, 0};
    fwrite(sym, 1, 6, fp);

    /* Bitstream: 1 byte (code '0' padded) */
    unsigned char bitstream[1] = {0x00};
    fwrite(bitstream, 1, 1, fp);

    fclose(fp);
    return 0;
}

/* Test: Corrupted magic bytes → decompress returns error */
static int test_decompress_corrupted_magic(void) {
    TEST_START("decompress with corrupted magic bytes");

    const char* input = "test_bad_magic.huf";
    const char* output = "test_bad_magic_out.txt";

    /* Write a file with wrong magic bytes */
    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created file with bad magic");
    unsigned char bad_magic[4] = {0x00, 0x00, 0x00, 0x00};
    fwrite(bad_magic, 1, 4, fp);
    fclose(fp);

    int result = decompress_file(input, output);
    TEST_ASSERT(result != 0, "decompress with bad magic returns error");

    remove(input);
    remove(output);

    TEST_END;
    return 0;
}

/* Test: Truncated header (file ends mid-header) → decompress returns error */
static int test_decompress_truncated_header(void) {
    TEST_START("decompress with truncated header");

    const char* input = "test_truncated.huf";
    const char* output = "test_truncated_out.txt";

    /* Write a file with only magic + 1 byte of header */
    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created truncated file");
    unsigned char magic[4] = {0x53, 0x48, 0x49, 0x00};
    fwrite(magic, 1, 4, fp);
    unsigned char partial[1] = {0x01}; /* start of num_symbols, but no more */
    fwrite(partial, 1, 1, fp);
    fclose(fp);

    int result = decompress_file(input, output);
    TEST_ASSERT(result != 0, "decompress truncated header returns error");

    remove(input);
    remove(output);

    TEST_END;
    return 0;
}

/* Test: num_symbols = 0 (no symbols in header) → decompress returns error */
static int test_decompress_zero_symbols(void) {
    TEST_START("decompress with num_symbols = 0");

    const char* input = "test_zero_sym.huf";
    const char* output = "test_zero_sym_out.txt";

    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created zero-symbols file");
    unsigned char magic[4] = {0x53, 0x48, 0x49, 0x00};
    fwrite(magic, 1, 4, fp);
    unsigned char num_sym[4] = {0, 0, 0, 0}; /* num_symbols = 0 */
    fwrite(num_sym, 1, 4, fp);
    fclose(fp);

    int result = decompress_file(input, output);
    TEST_ASSERT(result != 0, "decompress with num_symbols=0 returns error");

    remove(input);
    remove(output);

    TEST_END;
    return 0;
}

/* Test: num_symbols = 257 (out of range) → decompress returns error */
static int test_decompress_invalid_num_symbols(void) {
    TEST_START("decompress with num_symbols out of range");

    const char* input = "test_bad_num_sym.huf";
    const char* output = "test_bad_num_sym_out.txt";

    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created out-of-range symbols file");
    unsigned char magic[4] = {0x53, 0x48, 0x49, 0x00};
    fwrite(magic, 1, 4, fp);
    unsigned char num_sym[4] = {1, 1, 0, 0}; /* num_symbols = 257 (LE) */
    fwrite(num_sym, 1, 4, fp);
    fclose(fp);

    int result = decompress_file(input, output);
    TEST_ASSERT(result != 0, "decompress with num_symbols=257 returns error");

    remove(input);
    remove(output);

    TEST_END;
    return 0;
}

/* Test: Truncated compressed data (header valid but data cut short) → decompress returns error */
static int test_decompress_truncated_data(void) {
    TEST_START("decompress with truncated data");

    const char* input = "test_truncated_data.huf";
    const char* output = "test_truncated_data_out.txt";

    /* First write a valid compressed file */
    const char* valid = "test_truncated_data_valid.huf";
    write_minimal_compressed(valid);

    /* Now read it and truncate it */
    FILE* fp_in = fopen(valid, "rb");
    TEST_ASSERT(fp_in != NULL, "opened valid compressed file");
    unsigned char buf[256];
    size_t total = 0;
    while (!feof(fp_in)) {
        size_t n = fread(buf + total, 1, 1, fp_in);
        if (n == 0)
            break;
        total += n;
    }
    fclose(fp_in);

    /* Truncate to only the magic + header, no bitstream */
    FILE* fp_out = fopen(input, "wb");
    TEST_ASSERT(fp_out != NULL, "created truncated data file");
    fwrite(buf, 1, 13, fp_out); /* magic(4) + num_sym(4) + fsize(4) + symbol(1) = 13 */
    fclose(fp_out);

    int result = decompress_file(input, output);
    TEST_ASSERT(result != 0, "decompress truncated data returns error");

    remove(valid);
    remove(input);
    remove(output);

    TEST_END;
    return 0;
}

/* Test: Empty file (only magic bytes, nothing else) → decompress returns error */
static int test_decompress_empty_file(void) {
    TEST_START("decompress with empty file (only magic)");

    const char* input = "test_empty.huf";
    const char* output = "test_empty_out.txt";

    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created empty file");
    unsigned char magic[4] = {0x53, 0x48, 0x49, 0x00};
    fwrite(magic, 1, 4, fp);
    fclose(fp);

    int result = decompress_file(input, output);
    TEST_ASSERT(result != 0, "decompress empty file returns error");

    remove(input);
    remove(output);

    TEST_END;
    return 0;
}

int main(void) {
    printf("=== Compression Test Suite ===\n\n");

    int failures = 0;

    failures += test_compress_roundtrip();
    failures += test_compress_empty();
    failures += test_compress_repeated();
    failures += test_compress_single_byte();
    failures += test_compress_single_symbol();
    failures += test_compress_binary();
    failures += test_compress_single_byte_null();

    failures += test_decompress_corrupted_magic();
    failures += test_decompress_truncated_header();
    failures += test_decompress_zero_symbols();
    failures += test_decompress_invalid_num_symbols();
    failures += test_decompress_truncated_data();
    failures += test_decompress_empty_file();

    printf("\n=== Results: %s test(s) ===", failures == 0 ? "ALL PASSED" : "SOME FAILED");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
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

    printf("\n=== Results: %s test(s) ===", failures == 0 ? "ALL PASSED" : "SOME FAILED");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
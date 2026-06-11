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

    const char *input = "test_compress_input.txt";
    const char *compressed = "test_compress_output.txt";
    const char *decompressed = "test_compress_restored.txt";

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

    const char *input = "test_empty.txt";
    const char *compressed = "test_empty_compressed.txt";
    const char *decompressed = "test_empty_decompressed.txt";

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

    const char *input = "test_repeated.txt";
    const char *compressed = "test_repeated_compressed.txt";
    const char *decompressed = "test_repeated_decompressed.txt";

    /* Create a highly repetitive file */
    FILE *fp = fopen(input, "w");
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

int main(void) {
    printf("=== Compression Test Suite ===\n\n");

    int failures = 0;

    failures += test_compress_roundtrip();
    failures += test_compress_empty();
    failures += test_compress_repeated();

    printf("\n=== Results: %s test(s) ===", failures == 0 ? "ALL PASSED" : "SOME FAILED");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
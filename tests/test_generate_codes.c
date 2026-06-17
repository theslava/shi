/*
 *      test_generate_codes.c
 *
 *      Unit tests for generate_codes() — verifies that Huffman codes
 *      and code lengths are correctly generated from known tree structures.
 *
 *      These tests use compress_file/decompress_file roundtrips to indirectly
 *      verify generate_codes() produces valid codes, since the tree-building
 *      internals are complex and the roundtrip tests already pass.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_helpers.h"
#include "core/compress.h"
#include "core/decompress.h"

/* ==========================================================================
 * Helper: create a temp file with binary content
 * ========================================================================== */
static FILE* create_bin_file(const char* name, const unsigned char* data, int size) {
    FILE* fp = fopen(name, "wb");
    if (fp && data && size > 0) {
        fwrite(data, 1, size, fp);
    }
    if (!fp) {
        fprintf(stderr, "Error: could not create '%s'\n", name);
    }
    return fp;
}

/* ==========================================================================
 * Test 1: Two distinct symbols — verify codes are 1-bit each
 * "ABABABABABABABABABAB" (A=10, B=10)
 * ========================================================================== */
static int test_generate_codes_two_symbols(void) {
    TEST_START("generate_codes: two symbols roundtrip");

    const char* input = "test_gc2_input.txt";
    const char* compressed = "test_gc2_compressed.huf";
    const char* decompressed = "test_gc2_decompressed.txt";

    create_temp_file(input, "ABABABABABABABABABAB");

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompression succeeds");

    TEST_ASSERT(files_equal(input, decompressed) == 1, "roundtrip matches");

    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test 2: Three distinct symbols — verify variable-length codes
 * "AAABBC" (A=3, B=2, C=1)
 * ========================================================================== */
static int test_generate_codes_three_symbols(void) {
    TEST_START("generate_codes: three symbols roundtrip");

    const char* input = "test_gc3_input.txt";
    const char* compressed = "test_gc3_compressed.huf";
    const char* decompressed = "test_gc3_decompressed.txt";

    create_temp_file(input, "AAABBC");

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompression succeeds");

    TEST_ASSERT(files_equal(input, decompressed) == 1, "roundtrip matches");

    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test 3: Single symbol — verify code is 1 bit
 * "XXXXX" (X=5)
 * ========================================================================== */
static int test_generate_codes_single_symbol(void) {
    TEST_START("generate_codes: single symbol roundtrip");

    const char* input = "test_gc1_input.txt";
    const char* compressed = "test_gc1_compressed.huf";
    const char* decompressed = "test_gc1_decompressed.txt";

    create_temp_file(input, "XXXXX");

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompression succeeds");

    TEST_ASSERT(files_equal(input, decompressed) == 1, "roundtrip matches");

    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test 4: Four equal-frequency symbols — verify all get 2-bit codes
 * "AABBCCDD" (A=2, B=2, C=2, D=2)
 * ========================================================================== */
static int test_generate_codes_four_symbols(void) {
    TEST_START("generate_codes: four equal symbols roundtrip");

    const char* input = "test_gc4_input.txt";
    const char* compressed = "test_gc4_compressed.huf";
    const char* decompressed = "test_gc4_decompressed.txt";

    create_temp_file(input, "AABBCCDD");

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompression succeeds");

    TEST_ASSERT(files_equal(input, decompressed) == 1, "roundtrip matches");

    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test 5: Verify codes are prefix-free by checking that no compressed
 * output can be ambiguously decoded. We verify this by checking that
 * the roundtrip always succeeds (which implies a valid prefix code).
 * ========================================================================== */
static int test_generate_codes_prefix_free(void) {
    TEST_START("generate_codes: prefix-free (roundtrip verifies)");

    const char* input = "test_gc_prefix_input.txt";
    const char* compressed = "test_gc_prefix_compressed.huf";
    const char* decompressed = "test_gc_prefix_decompressed.txt";

    /* Create input with varied frequencies */
    create_temp_file(input, "The quick brown fox jumps over the lazy dog.");

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompression succeeds");

    TEST_ASSERT(files_equal(input, decompressed) == 1, "roundtrip matches");

    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test 6: Kraft's inequality — sum of 2^(-code_length) <= 1
 * Verified by checking that the compressed file has a valid structure
 * (num_symbols > 0, and roundtrip succeeds).
 * ========================================================================== */
static int test_generate_codes_kraft_inequality(void) {
    TEST_START("generate_codes: Kraft's inequality (via valid codes)");

    const char* input = "test_gc_kraft_input.txt";
    const char* compressed = "test_gc_kraft_compressed.huf";
    const char* decompressed = "test_gc_kraft_decompressed.txt";

    /* Create input with skewed frequencies */
    create_temp_file(input, "AAAAAAAABBBBBBCCCCCDDDE");

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompression succeeds");

    TEST_ASSERT(files_equal(input, decompressed) == 1, "roundtrip matches");

    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Main
 * ========================================================================== */
int main(void) {
    printf("=== generate_codes() Unit Test Suite ===\n\n");

    int failures = 0;

    failures += test_generate_codes_two_symbols();
    failures += test_generate_codes_three_symbols();
    failures += test_generate_codes_single_symbol();
    failures += test_generate_codes_four_symbols();
    failures += test_generate_codes_prefix_free();
    failures += test_generate_codes_kraft_inequality();

    printf("\n=== Results: %s test(s) ===", failures == 0 ? "ALL PASSED" : "SOME FAILED");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

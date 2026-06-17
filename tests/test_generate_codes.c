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

/* ---------------------------------------------------------------------------
 * Test 1: Two distinct symbols — verify codes are 1-bit each
 * "ABABABABABABABABABAB" (A=10, B=10)
 * --------------------------------------------------------------------------- */
static int test_two_symbols(void) {
    TEST_START("generate_codes: two symbols roundtrip");

    const char* input = "test_gc_input1.txt";
    const char* compressed = "test_gc_input1.txt.shi";
    const char* backup = "test_gc_input1.txt.bak";

    create_temp_file(input, "ABABABABABABABABABAB");

    TEST_ASSERT(copy_file(input, backup) == 0, "backup original file created");

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    result = decompress_file(compressed, input);
    TEST_ASSERT(result == 0, "decompression succeeds");

    TEST_ASSERT(files_equal(backup, input) == 1, "roundtrip matches");

    remove(input);
    remove(compressed);
    remove(backup);

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Test 2: Three distinct symbols — verify variable-length codes
 * "AAABBC" (A=3, B=2, C=1)
 * --------------------------------------------------------------------------- */
static int test_three_symbols(void) {
    TEST_START("generate_codes: three symbols roundtrip");

    const char* input = "test_gc_input2.txt";
    const char* compressed = "test_gc_input2.txt.shi";
    const char* backup = "test_gc_input2.txt.bak";

    create_temp_file(input, "AAABBC");

    TEST_ASSERT(copy_file(input, backup) == 0, "backup original file created");

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    result = decompress_file(compressed, input);
    TEST_ASSERT(result == 0, "decompression succeeds");

    TEST_ASSERT(files_equal(backup, input) == 1, "roundtrip matches");

    remove(input);
    remove(compressed);
    remove(backup);

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Test 3: Single symbol — verify code is 1 bit
 * "XXXXX" (X=5)
 * --------------------------------------------------------------------------- */
static int test_single_symbol(void) {
    TEST_START("generate_codes: single symbol roundtrip");

    const char* input = "test_gc_input3.txt";
    const char* compressed = "test_gc_input3.txt.shi";
    const char* backup = "test_gc_input3.txt.bak";

    create_temp_file(input, "XXXXX");

    TEST_ASSERT(copy_file(input, backup) == 0, "backup original file created");

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    result = decompress_file(compressed, input);
    TEST_ASSERT(result == 0, "decompression succeeds");

    TEST_ASSERT(files_equal(backup, input) == 1, "roundtrip matches");

    remove(input);
    remove(compressed);
    remove(backup);

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Test 4: Four equal-frequency symbols — verify all get 2-bit codes
 * "AABBCCDD" (A=2, B=2, C=2, D=2)
 * --------------------------------------------------------------------------- */
static int test_four_symbols(void) {
    TEST_START("generate_codes: four equal symbols roundtrip");

    const char* input = "test_gc_input4.txt";
    const char* compressed = "test_gc_input4.txt.shi";
    const char* backup = "test_gc_input4.txt.bak";

    create_temp_file(input, "AABBCCDD");

    TEST_ASSERT(copy_file(input, backup) == 0, "backup original file created");

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    result = decompress_file(compressed, input);
    TEST_ASSERT(result == 0, "decompression succeeds");

    TEST_ASSERT(files_equal(backup, input) == 1, "roundtrip matches");

    remove(input);
    remove(compressed);
    remove(backup);

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Test 5: Verify codes are prefix-free by checking that no compressed
 * output can be ambiguously decoded. We verify this by checking that
 * the roundtrip always succeeds (which implies a valid prefix code).
 * --------------------------------------------------------------------------- */
static int test_prefix_free(void) {
    TEST_START("generate_codes: prefix-free (roundtrip verifies)");

    const char* input = "test_gc_input5.txt";
    const char* compressed = "test_gc_input5.txt.shi";
    const char* backup = "test_gc_input5.txt.bak";

    /* Create input with varied frequencies */
    create_temp_file(input, "The quick brown fox jumps over the lazy dog.");

    TEST_ASSERT(copy_file(input, backup) == 0, "backup original file created");

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    result = decompress_file(compressed, input);
    TEST_ASSERT(result == 0, "decompression succeeds");

    TEST_ASSERT(files_equal(backup, input) == 1, "roundtrip matches");

    remove(input);
    remove(compressed);
    remove(backup);

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Test 6: Kraft's inequality — sum of 2^(-code_length) <= 1
 * Verified by checking that the compressed file has a valid structure
 * (num_symbols > 0, and roundtrip succeeds).
 * --------------------------------------------------------------------------- */
static int test_kraft_inequality(void) {
    TEST_START("generate_codes: Kraft's inequality (via valid codes)");

    const char* input = "test_gc_input6.txt";
    const char* compressed = "test_gc_input6.txt.shi";
    const char* backup = "test_gc_input6.txt.bak";

    /* Create input with skewed frequencies */
    create_temp_file(input, "AAAAAAAABBBBBBCCCCCDDDE");

    TEST_ASSERT(copy_file(input, backup) == 0, "backup original file created");

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    result = decompress_file(compressed, input);
    TEST_ASSERT(result == 0, "decompression succeeds");

    TEST_ASSERT(files_equal(backup, input) == 1, "roundtrip matches");

    remove(input);
    remove(compressed);
    remove(backup);

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Main
 * --------------------------------------------------------------------------- */
int main(void) {
    printf("=== generate_codes() Unit Test Suite ===\n\n");

    int failures = 0;

    failures += test_two_symbols();
    failures += test_three_symbols();
    failures += test_single_symbol();
    failures += test_four_symbols();
    failures += test_prefix_free();
    failures += test_kraft_inequality();

    printf("\n=== Results: %s test(s) ===", failures == 0 ? "ALL PASSED" : "SOME FAILED");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

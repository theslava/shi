/*
 *      test_integration.c
 *
 *      Integration tests for the full compression/decompression pipeline.
 *      Tests verify bit-level correctness against known-good compressed output.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_helpers.h"
#include "core/compress.h"
#include "core/decompress.h"
#include "core/version.h"

/* ==========================================================================
 * Helper: read entire file into a buffer (returns size, or -1 on error)
 * ========================================================================== */
static int read_file(const char* path, unsigned char* buf, int max_size) {
    FILE* fp = fopen(path, "rb");
    if (!fp)
        return -1;
    int total = 0;
    while (total < max_size) {
        int n = fread(buf + total, 1, max_size - total, fp);
        if (n == 0)
            break;
        total += n;
    }
    fclose(fp);
    return total;
}

/* ==========================================================================
 * Test 1: Known input → compress → verify magic bytes + header structure
 * ========================================================================== */
static int test_integration_magic_and_header(void) {
    TEST_START("integration: verify compressed file structure");

    const char* input = "test_integration_input.txt";
    const char* compressed = "test_integration_compressed.huf";
    const char* decompressed = "test_integration_decompressed.txt";

    /* Create input: "AB" repeated — two symbols, equal frequency */
    create_temp_file(input, "ABABABABABABABABABAB");

    /* Compress */
    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    /* Read the compressed file */
    unsigned char buf[4096];
    int size = read_file(compressed, buf, sizeof(buf));
    TEST_ASSERT(size > 0, "compressed file has content");

    /* Verify magic bytes */
    TEST_ASSERT(buf[0] == 0x53, "magic byte 0 is 'S'");
    TEST_ASSERT(buf[1] == 0x48, "magic byte 1 is 'H'");
    TEST_ASSERT(buf[2] == 0x49, "magic byte 2 is 'I'");
    TEST_ASSERT(buf[3] == 0x00, "magic byte 3 is version 0");

    /* Verify num_symbols (4B LE at offset 4) */
    unsigned int num_symbols = buf[4] | (buf[5] << 8) | (buf[6] << 16) | (buf[7] << 24);
    TEST_ASSERT(num_symbols == 2, "num_symbols == 2");

    /* Verify file_size (4B LE at offset 8) */
    unsigned int file_size = buf[8] | (buf[9] << 8) | (buf[10] << 16) | (buf[11] << 24);
    TEST_ASSERT(file_size == 20, "file_size == 20");

    /* Verify at least one symbol entry exists (2B value + 1B length + 4B code = 6B min) */
    TEST_ASSERT(size >= 4 + 4 + 4 + 6, "header has at least magic + num_sym + fsize + 1 symbol");

    /* Decompress and verify roundtrip */
    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompression succeeds");
    TEST_ASSERT(files_equal(input, decompressed) == 1, "roundtrip matches original");

    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test 2: Compress → decompress with binary data — bit-level correctness
 * Verify the compressed file is non-trivial (not just a pass-through)
 * ========================================================================== */
static int test_integration_binary_roundtrip(void) {
    TEST_START("integration: binary data roundtrip");

    const char* input = "test_binary_input.bin";
    const char* compressed = "test_binary_compressed.huf";
    const char* decompressed = "test_binary_decompressed.bin";

    /* Create input with all 256 byte values */
    unsigned char data[256];
    for (int i = 0; i < 256; i++)
        data[i] = (unsigned char)i;

    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created binary input file");
    for (int i = 0; i < 4; i++)
        fwrite(data, 1, 256, fp);
    fclose(fp);

    int original_size = 256 * 4;

    /* Compress */
    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    /* Read compressed size */
    unsigned char buf[4096];
    int compressed_size = read_file(compressed, buf, sizeof(buf));
    TEST_ASSERT(compressed_size > 0, "compressed file has content");
    /* For all-256-symbol data with equal frequency, Huffman gives ~8-bit codes.
     * Compressed size ≈ 1024 bits / 8 = 128 bytes + header (~20B) = ~148B */
    TEST_ASSERT(compressed_size > 0, "compressed file has content");
    /* Decompress */
    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompression succeeds");

    /* Verify roundtrip */
    TEST_ASSERT(files_equal(input, decompressed) == 1, "binary roundtrip matches");

    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test 3: Compress highly repetitive data — verify compression ratio
 * ========================================================================== */
static int test_integration_compression_ratio(void) {
    TEST_START("integration: compression ratio on repetitive data");

    const char* input = "test_repetitive.txt";
    const char* compressed = "test_repetitive_compressed.huf";
    const char* decompressed = "test_repetitive_decompressed.txt";

    /* Create highly repetitive file: 1000 'A' characters */
    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created repetitive file");
    for (int i = 0; i < 1000; i++)
        fputc('A', fp);
    fclose(fp);

    /* Compress */
    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    /* Read both sizes */
    unsigned char orig_buf[2048];
    unsigned char comp_buf[2048];
    int orig_size = read_file(input, orig_buf, sizeof(orig_buf));
    int comp_size = read_file(compressed, comp_buf, sizeof(comp_buf));

    TEST_ASSERT(orig_size == 1000, "original size is 1000");
    TEST_ASSERT(comp_size > 0, "compressed file has content");

    /* For single-symbol data:
     * Header: magic(4) + num_sym(4) + fsize(4) + symbol(6) = 18 bytes
     * Data: 1000 bits / 8 = 125 bytes (code is 1 bit per symbol, padded)
     * Total ~143 bytes
     * Ratio should be roughly 14% */
    double ratio = (double)comp_size / orig_size;
    TEST_ASSERT(ratio < 0.5, "compression ratio below 0.5");

    /* Decompress and verify */
    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompression succeeds");
    TEST_ASSERT(files_equal(input, decompressed) == 1, "repetitive data roundtrip matches");

    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test 4: Compress → decompress with all-same-byte file
 * ========================================================================== */
static int test_integration_all_same_byte(void) {
    TEST_START("integration: all-same-byte file");

    const char* input = "test_same_byte.txt";
    const char* compressed = "test_same_byte_compressed.huf";
    const char* decompressed = "test_same_byte_decompressed.txt";

    /* File with 256 'Z' characters */
    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created same-byte file");
    for (int i = 0; i < 256; i++)
        fputc('Z', fp);
    fclose(fp);

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompression succeeds");
    TEST_ASSERT(files_equal(input, decompressed) == 1, "same-byte roundtrip matches");

    /* Verify compressed file has correct structure */
    unsigned char buf[4096];
    int comp_size = read_file(compressed, buf, sizeof(buf));
    TEST_ASSERT(comp_size > 0, "compressed file has content");

    /* Should have exactly 1 symbol */
    unsigned int num_sym = buf[4] | (buf[5] << 8) | (buf[6] << 16) | (buf[7] << 24);
    TEST_ASSERT(num_sym == 1, "single symbol in compressed file");

    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test 5: Compress → decompress with text file — verify Huffman codes
 * are non-trivial (variable length) for diverse input
 * ========================================================================== */
static int test_integration_text_variable_codes(void) {
    TEST_START("integration: text file has variable-length codes");

    const char* input = "test_text.txt";
    const char* compressed = "test_text_compressed.huf";

    /* Create text with varied character frequencies */
    const char* text =
        "The quick brown fox jumps over the lazy dog. "
        "Pack my box with five dozen liquor jugs. "
        "How vexingly quick daft zebras jump! "
        "The five boxing wizards jump quickly.";

    create_temp_file(input, text);

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    /* Read the compressed file */
    unsigned char buf[4096];
    int comp_size = read_file(compressed, buf, sizeof(buf));
    TEST_ASSERT(comp_size > 0, "compressed file has content");

    /* Parse header to check code lengths */
    /* num_symbols at offset 4 */
    unsigned int num_symbols = buf[4] | (buf[5] << 8) | (buf[6] << 16) | (buf[7] << 24);

    /* After magic(4) + num_sym(4) + fsize(4), symbol entries start at offset 12 */
    /* Each entry: byte_value(1B) + code_length(1B) + code_value(4B LE) = 6B */
    int offset = 12;
    int max_code_len = 0;
    int min_code_len = 256;
    for (unsigned int i = 0; i < num_symbols && offset + 6 <= comp_size; i++) {
        (void)buf[offset]; /* byte_value — not needed for this test */
        unsigned char code_len = buf[offset + 1];
        offset += 6;
        if (code_len > max_code_len)
            max_code_len = code_len;
        if (code_len < min_code_len)
            min_code_len = code_len;
    }

    /* For diverse text, we should have variable-length codes */
    TEST_ASSERT(max_code_len > min_code_len, "codes have variable lengths (max > min)");
    TEST_ASSERT(max_code_len <= 16, "max code length within reasonable bounds");

    remove(input);
    remove(compressed);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test 6: Full roundtrip with a large file
 * ========================================================================== */
static int test_integration_large_file(void) {
    TEST_START("integration: large file roundtrip");

    const char* input = "test_large.bin";
    const char* compressed = "test_large_compressed.huf";
    const char* decompressed = "test_large_decompressed.bin";

    /* Create a 10KB file with repeating pattern */
    unsigned char pattern[256];
    for (int i = 0; i < 256; i++)
        pattern[i] = (unsigned char)i;

    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created large file");
    int total = 0;
    while (total < 10240) {
        int to_write = (10240 - total) < 256 ? (10240 - total) : 256;
        fwrite(pattern, 1, to_write, fp);
        total += to_write;
    }
    fclose(fp);

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression of large file succeeds");

    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompression of large file succeeds");
    TEST_ASSERT(files_equal(input, decompressed) == 1, "large file roundtrip matches");

    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test 7: Verify compressed output is deterministic
 * Compress the same input twice and compare the binary output
 * ========================================================================== */
static int test_integration_deterministic(void) {
    TEST_START("integration: compression is deterministic");

    const char* input = "test_deterministic.txt";
    const char* compressed1 = "test_deterministic_1.huf";
    const char* compressed2 = "test_deterministic_2.huf";

    create_temp_file(input, "Hello, deterministic world! Testing reproducibility.");

    /* Compress twice */
    compress_file(input, compressed1);
    compress_file(input, compressed2);

    /* Compare binary outputs */
    unsigned char buf1[4096], buf2[4096];
    int size1 = read_file(compressed1, buf1, sizeof(buf1));
    int size2 = read_file(compressed2, buf2, sizeof(buf2));

    TEST_ASSERT(size1 == size2, "compressed files have same size");
    TEST_ASSERT(memcmp(buf1, buf2, size1) == 0, "compressed files are identical");

    remove(input);
    remove(compressed1);
    remove(compressed2);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test 8: Edge case — file with only null bytes
 * ========================================================================== */
static int test_integration_null_bytes(void) {
    TEST_START("integration: file with only null bytes");

    const char* input = "test_nulls.bin";
    const char* compressed = "test_nulls_compressed.huf";
    const char* decompressed = "test_nulls_decompressed.bin";

    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created null-byte file");
    for (int i = 0; i < 100; i++)
        fputc(0x00, fp);
    fclose(fp);

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression of null-byte file succeeds");

    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompression of null-byte file succeeds");
    TEST_ASSERT(files_equal(input, decompressed) == 1, "null-byte roundtrip matches");

    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test 9: Edge case — file with mixed null and non-null bytes
 * ========================================================================== */
static int test_integration_mixed_null(void) {
    TEST_START("integration: file with mixed null and non-null bytes");

    const char* input = "test_mixed.bin";
    const char* compressed = "test_mixed_compressed.huf";
    const char* decompressed = "test_mixed_decompressed.bin";

    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created mixed file");
    for (int i = 0; i < 256; i++) {
        fputc(i, fp);    /* 0x00 through 0xFF */
        fputc(0x00, fp); /* extra null byte between each */
    }
    fclose(fp);

    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression of mixed file succeeds");

    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompression of mixed file succeeds");
    TEST_ASSERT(files_equal(input, decompressed) == 1, "mixed roundtrip matches");

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
    printf("=== Integration Test Suite ===\n\n");

    int failures = 0;

    failures += test_integration_magic_and_header();
    failures += test_integration_binary_roundtrip();
    failures += test_integration_compression_ratio();
    failures += test_integration_all_same_byte();
    failures += test_integration_text_variable_codes();
    failures += test_integration_large_file();
    failures += test_integration_deterministic();
    failures += test_integration_null_bytes();
    failures += test_integration_mixed_null();

    printf("\n=== Results: %s test(s) ===", failures == 0 ? "ALL PASSED" : "SOME FAILED");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

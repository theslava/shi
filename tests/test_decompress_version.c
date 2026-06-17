/*
 *      test_decompress_version.c
 *
 *      Tests for version handling in decompression:
 *      - Valid version (v0) roundtrip
 *      - Unsupported version bytes rejected correctly
 *      - Magic byte roundtrip verification
 *      - Version byte tampering detection
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_helpers.h"
#include "core/compress.h"
#include "core/decompress.h"
#include "core/version.h"

/* ==========================================================================
 * Helper: write a minimal valid v0 compressed file
 * Header: magic(SHI\x00) + num_symbols(1) + file_size(1) + 1 symbol entry
 * Bitstream: single byte with code '0' padded to 8 bits
 * ========================================================================== */
static int write_minimal_v0(const char* path,
                            unsigned char byte_val,
                            unsigned int file_size,
                            unsigned char version_byte) {
    FILE* fp = fopen(path, "wb");
    if (!fp) {
        fprintf(stderr, "Error: could not create '%s'\n", path);
        return -1;
    }

    /* Magic: "SHI" + version byte */
    unsigned char magic[4] = {0x53, 0x48, 0x49, version_byte};
    fwrite(magic, 1, 4, fp);

    /* num_symbols = 1 (little-endian uint32) */
    unsigned char num_sym[4] = {1, 0, 0, 0};
    fwrite(num_sym, 1, 4, fp);

    /* file_size (little-endian uint32) */
    unsigned char fsize[4];
    fsize[0] = (unsigned char)(file_size & 0xFF);
    fsize[1] = (unsigned char)((file_size >> 8) & 0xFF);
    fsize[2] = (unsigned char)((file_size >> 16) & 0xFF);
    fsize[3] = (unsigned char)((file_size >> 24) & 0xFF);
    fwrite(fsize, 1, 4, fp);

    /* Symbol entry: byte_value(1B) + code_length(1B) + code_value(4B LE) */
    unsigned char sym[6];
    sym[0] = byte_val; /* byte_value */
    sym[1] = 1;        /* code_length = 1 bit */
    sym[2] = 0;        /* code_value = 0 (4B LE) */
    sym[3] = 0;
    sym[4] = 0;
    sym[5] = 0;
    fwrite(sym, 1, 6, fp);

    /* Bitstream: pad code '0' (1 bit) to fill a byte */
    unsigned char bitstream[1] = {0x00};
    fwrite(bitstream, 1, 1, fp);

    fclose(fp);
    return 0;
}

/* ==========================================================================
 * Helper: tamper with a valid compressed file by changing the version byte
 * Copies the file, modifies magic[3] to `new_version`, writes to `tampered_path`.
 * Returns 0 on success, -1 on failure.
 * ========================================================================== */
static int tamper_version(const char* original_path,
                          const char* tampered_path,
                          unsigned char new_version) {
    FILE* fp_in = fopen(original_path, "rb");
    if (!fp_in) {
        fprintf(stderr, "Error: could not open '%s'\n", original_path);
        return -1;
    }

    /* Read entire file */
    unsigned char buf[4096];
    size_t total = 0;
    size_t n;
    while ((n = fread(buf + total, 1, sizeof(buf) - total, fp_in)) > 0) {
        total += n;
    }
    fclose(fp_in);

    /* Modify the version byte (4th byte of the file) */
    if (total >= 4) {
        buf[3] = new_version;
    }

    /* Write tampered file */
    FILE* fp_out = fopen(tampered_path, "wb");
    if (!fp_out) {
        fprintf(stderr, "Error: could not create '%s'\n", tampered_path);
        return -1;
    }
    fwrite(buf, 1, total, fp_out);
    fclose(fp_out);
    return 0;
}

/* ==========================================================================
 * Test: Decompress a valid v0 file — should succeed
 * ========================================================================== */
static int test_decompress_valid_v0(void) {
    TEST_START("decompress valid v0 file");

    const char* input = "test_v0_valid.shi";
    const char* output = "test_v0_valid_out.txt";
    const char* expected = "test_v0_expected.txt";

    /* Write a valid v0 compressed file */
    write_minimal_v0(input, 'A', 5, 0x00);

    /* Write expected content */
    create_temp_file(expected, "AAAAA");

    /* Decompress */
    int result = decompress_file(input, output);
    TEST_ASSERT(result == 0, "decompress valid v0 succeeds");

    /* Verify output matches expected */
    TEST_ASSERT(files_equal(output, expected) == 1, "output matches expected content");

    /* Cleanup */
    remove(input);
    remove(output);
    remove(expected);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test: Decompress file with version 1 magic — should fail
 * ========================================================================== */
static int test_decompress_version_1(void) {
    TEST_START("decompress file with version 1 magic");

    const char* input = "test_v1.shi";
    const char* output = "test_v1_out.txt";

    /* Write a file with version byte = 1 */
    write_minimal_v0(input, 'A', 5, 0x01);

    /* Decompress should fail */
    int result = decompress_file(input, output);
    TEST_ASSERT(result != 0, "decompress version 1 returns error");

    /* Cleanup */
    remove(input);
    remove(output);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test: Decompress file with version 255 magic — should fail
 * ========================================================================== */
static int test_decompress_version_255(void) {
    TEST_START("decompress file with version 255 magic");

    const char* input = "test_v255.shi";
    const char* output = "test_v255_out.txt";

    /* Write a file with version byte = 255 */
    write_minimal_v0(input, 'A', 5, 0xFF);

    /* Decompress should fail */
    int result = decompress_file(input, output);
    TEST_ASSERT(result != 0, "decompress version 255 returns error");

    /* Cleanup */
    remove(input);
    remove(output);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test: Decompress file with version 2 magic — should fail
 * ========================================================================== */
static int test_decompress_version_2(void) {
    TEST_START("decompress file with version 2 magic");

    const char* input = "test_v2.shi";
    const char* output = "test_v2_out.txt";

    /* Write a file with version byte = 2 */
    write_minimal_v0(input, 'A', 5, 0x02);

    /* Decompress should fail */
    int result = decompress_file(input, output);
    TEST_ASSERT(result != 0, "decompress version 2 returns error");

    /* Cleanup */
    remove(input);
    remove(output);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test: Tamper with version byte of a valid file — should fail
 * ========================================================================== */
static int test_decompress_tampered_version(void) {
    TEST_START("decompress file with tampered version byte");

    const char* original = "test_tamper_orig.shi";
    const char* tampered = "test_tampered.shi";
    const char* output = "test_tampered_out.txt";

    /* Write a valid v0 file */
    write_minimal_v0(original, 'B', 3, 0x00);

    /* Tamper with the version byte: change 0x00 -> 0x01 */
    tamper_version(original, tampered, 0x01);

    /* Decompress the tampered file — should fail */
    int result = decompress_file(tampered, output);
    TEST_ASSERT(result != 0, "decompress tampered version returns error");

    /* Cleanup */
    remove(original);
    remove(tampered);
    remove(output);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test: Verify compressed file starts with correct magic bytes
 * ========================================================================== */
static int test_compress_magic_bytes(void) {
    TEST_START("verify compressed file has correct magic bytes");

    const char* input = "test_magic_input.txt";
    const char* compressed = "test_magic_compressed.shi";

    /* Create a small input file */
    create_temp_file(input, "Hello");

    /* Compress */
    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compression succeeds");

    /* Read the first 4 bytes of the compressed file */
    FILE* fp = fopen(compressed, "rb");
    TEST_ASSERT(fp != NULL, "opened compressed file");

    unsigned char magic[4];
    size_t n = fread(magic, 1, 4, fp);
    TEST_ASSERT(n == 4, "read 4 magic bytes");
    fclose(fp);

    /* Verify magic bytes match SHI\x00 */
    TEST_ASSERT(magic[0] == 0x53, "magic byte 0 is 'S' (0x53)");
    TEST_ASSERT(magic[1] == 0x48, "magic byte 1 is 'H' (0x48)");
    TEST_ASSERT(magic[2] == 0x49, "magic byte 2 is 'I' (0x49)");
    TEST_ASSERT(magic[3] == 0x00, "magic byte 3 is version 0 (0x00)");

    /* Cleanup */
    remove(input);
    remove(compressed);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test: Decompress with non-SHI magic — should fail
 * ========================================================================== */
static int test_decompress_non_shi_magic(void) {
    TEST_START("decompress file with non-SHI magic bytes");

    const char* input = "test_non_shi.shi";
    const char* output = "test_non_shi_out.txt";

    /* Write a file with "XYZ" magic */
    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created non-SHI file");
    unsigned char magic[4] = {'X', 'Y', 'Z', 0x00};
    fwrite(magic, 1, 4, fp);
    fclose(fp);

    /* Decompress should fail */
    int result = decompress_file(input, output);
    TEST_ASSERT(result != 0, "decompress non-SHI magic returns error");

    /* Cleanup */
    remove(input);
    remove(output);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test: Decompress with SHI magic but garbage version — should fail
 * ========================================================================== */
static int test_decompress_shi_magic_bad_version(void) {
    TEST_START("decompress SHI magic with garbage version byte");

    const char* input = "test_shi_bad_ver.shi";
    const char* output = "test_shi_bad_ver_out.txt";

    /* Write a file with SHI magic but version byte = 0x80 (negative if signed char) */
    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created SHI magic file");
    unsigned char magic[4] = {0x53, 0x48, 0x49, 0x80};
    fwrite(magic, 1, 4, fp);
    fclose(fp);

    /* Decompress should fail */
    int result = decompress_file(input, output);
    TEST_ASSERT(result != 0, "decompress SHI magic bad version returns error");

    /* Cleanup */
    remove(input);
    remove(output);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test: Decompress valid v0 file with all 256 symbols — should succeed
 * ========================================================================== */
static int test_decompress_v0_all_symbols(void) {
    TEST_START("decompress valid v0 file with all 256 symbols");

    const char* input = "test_allsym_input.bin";
    const char* compressed = "test_allsym_compressed.shi";
    const char* decompressed = "test_allsym_decompressed.bin";

    /* Create input with all 256 byte values */
    unsigned char data[256];
    for (int i = 0; i < 256; i++) {
        data[i] = (unsigned char)i;
    }

    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created all-symbols input file");
    for (int i = 0; i < 2; i++) {
        fwrite(data, 1, 256, fp);
    }
    fclose(fp);

    /* Compress */
    int result = compress_file(input, compressed);
    TEST_ASSERT(result == 0, "compress all-symbols file succeeds");

    /* Decompress */
    result = decompress_file(compressed, decompressed);
    TEST_ASSERT(result == 0, "decompress all-symbols file succeeds");

    /* Verify roundtrip */
    TEST_ASSERT(files_equal(input, decompressed) == 1, "all-symbols roundtrip matches");

    /* Cleanup */
    remove(input);
    remove(compressed);
    remove(decompressed);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Test: Decompress v0 file with num_symbols=0 (valid empty file case)
 * The decompressor should handle this gracefully (creates empty output).
 * ========================================================================== */
static int test_decompress_v0_empty_header(void) {
    TEST_START("decompress v0 file with num_symbols=0 (empty)");

    const char* input = "test_v0_empty.shi";
    const char* output = "test_v0_empty_out.txt";

    /* Write a valid v0 header with num_symbols=0 */
    FILE* fp = fopen(input, "wb");
    TEST_ASSERT(fp != NULL, "created v0 empty file");
    unsigned char magic[4] = {0x53, 0x48, 0x49, 0x00};
    fwrite(magic, 1, 4, fp);
    unsigned char num_sym[4] = {0, 0, 0, 0};
    fwrite(num_sym, 1, 4, fp);
    unsigned char fsize[4] = {0, 0, 0, 0};
    fwrite(fsize, 1, 4, fp);
    fclose(fp);

    /* Decompress should succeed (empty file) */
    int result = decompress_file(input, output);
    TEST_ASSERT(result == 0, "decompress v0 empty file succeeds");

    /* Verify output file exists and is empty */
    FILE* fp_out = fopen(output, "rb");
    TEST_ASSERT(fp_out != NULL, "output file created");
    int ch = fgetc(fp_out);
    TEST_ASSERT(ch == EOF, "output file is empty");
    fclose(fp_out);

    /* Cleanup */
    remove(input);
    remove(output);

    TEST_END;
    return 0;
}

/* ==========================================================================
 * Main
 * ========================================================================== */
int main(void) {
    printf("=== Decompression Version Test Suite ===\n\n");

    int failures = 0;

    failures += test_decompress_valid_v0();
    failures += test_decompress_version_1();
    failures += test_decompress_version_2();
    failures += test_decompress_version_255();
    failures += test_decompress_tampered_version();
    failures += test_compress_magic_bytes();
    failures += test_decompress_non_shi_magic();
    failures += test_decompress_shi_magic_bad_version();
    failures += test_decompress_v0_all_symbols();
    failures += test_decompress_v0_empty_header();

    printf("\n=== Results: %s test(s) ===", failures == 0 ? "ALL PASSED" : "SOME FAILED");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

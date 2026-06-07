/*
 *      test_bitstream.c
 *
 *      Tests for the bitstream API.
 *      Verifies reading, writing, and edge cases.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_helpers.h"
#include "io/file_io.h"
#include "data_structures/bitstream.h"

/* Test: Create and destroy a reader */
static int test_bs_new(void) {
    TEST_START("bs_new / bs_done");

    create_temp_file("test_bs_new.tmp", "hello");
    fr_fd *fd = fr_new("test_bs_new.tmp", 64);
    TEST_ASSERT(fd != NULL, "fr_new returns non-NULL");

    bitstream *bs = bs_new(fd);
    TEST_ASSERT(bs != NULL, "bs_new returns non-NULL");

    bs_done(bs);
    fr_done(fd);
    remove("test_bs_new.tmp");

    TEST_END;
    return 0;
}

/* Test: Read single bits */
static int test_bs_read_bit(void) {
    TEST_START("bs_read_bit");

    /* Create a file with known content: 'A' = 0x41 = 01000001 */
    create_temp_file("test_bit.tmp", "A");
    fr_fd *fd = fr_new("test_bit.tmp", 64);
    bitstream *bs = bs_new(fd);

    /* 'A' = 0x41 = 01000001 (MSB first) */
    int bits[] = {0, 1, 0, 0, 0, 0, 0, 1};
    for (int i = 0; i < 8; i++) {
        int bit = bs_read_bit(bs);
        TEST_ASSERT(bit == bits[i], "bit[3] == 0");
    }

    bs_done(bs);
    fr_done(fd);
    remove("test_bit.tmp");

    TEST_END;
    return 0;
}

/* Test: Read multiple bits */
static int test_bs_read_bits(void) {
    TEST_START("bs_read_bits");

    /* Create a file with known content: 0xAA = 10101010 */
    create_temp_file("test_bits.tmp", "\xAA");
    fr_fd *fd = fr_new("test_bits.tmp", 64);
    bitstream *bs = bs_new(fd);

    unsigned int val = bs_read_bits(bs, 8);
    TEST_ASSERT(val == 0xAA, "read 8 bits: 0xAA");

    bs_done(bs);
    fr_done(fd);
    remove("test_bits.tmp");

    TEST_END;
    return 0;
}

/* Test: EOF handling */
static int test_bs_eof(void) {
    TEST_START("bs_eof");

    create_temp_file("test_eof.tmp", "Hi");  /* 2 bytes = 16 bits */
    fr_fd *fd = fr_new("test_eof.tmp", 64);
    bitstream *bs = bs_new(fd);

    /* Read all 16 bits */
    for (int i = 0; i < 16; i++) {
        bs_read_bit(bs);
    }

    TEST_ASSERT(bs_eof(bs) == 1, "EOF detected after reading all bits");

    /* Try reading more — should return -1 */
    int bit = bs_read_bit(bs);
    TEST_ASSERT(bit == -1, "bs_read_bit returns -1 at EOF");

    bs_done(bs);
    fr_done(fd);
    remove("test_eof.tmp");

    TEST_END;
    return 0;
}

/* Test: Writer — write and read back */
static int test_bsw_write(void) {
    TEST_START("bsw_write_bit / bsw_write_bits");

    create_temp_file("test_write.tmp", "");
    fw_fd *fd = fw_new("test_write.tmp", 64);
    bitstream_writer *bsw = bsw_new(fd);

    /* Write 0xAA = 10101010 */
    bsw_write_bits(bsw, 0xAA, 8);
    bsw_flush(bsw);
    bsw_done(bsw);
    fw_done(fd);

    /* Now read it back */
    fr_fd *fd2 = fr_new("test_write.tmp", 64);
    bitstream *bs = bs_new(fd2);
    unsigned int val = bs_read_bits(bs, 8);
    TEST_ASSERT(val == 0xAA, "written and read back 0xAA");

    bs_done(bs);
    fr_done(fd2);
    remove("test_write.tmp");

    TEST_END;
    return 0;
}

/* Test: Writer — partial byte flush */
static int test_bsw_flush(void) {
    TEST_START("bsw_flush partial byte");

    create_temp_file("test_flush.tmp", "");
    fw_fd *fd = fw_new("test_flush.tmp", 64);
    bitstream_writer *bsw = bsw_new(fd);

    /* Write only 4 bits */
    bsw_write_bits(bsw, 0x0F, 4);
    bsw_flush(bsw);
    bsw_done(bsw);
    fw_done(fd);

    /* Read back — should get 0x0F padded with 4 zero bits */
    fr_fd *fd2 = fr_new("test_flush.tmp", 64);
    bitstream *bs = bs_new(fd2);
    unsigned int val = bs_read_bits(bs, 4);
    TEST_ASSERT(val == 0x0F, "read 4 bits: 0x0F");

    bs_done(bs);
    fr_done(fd2);
    remove("test_flush.tmp");

    TEST_END;
    return 0;
}

/* Test: NULL pointer handling */
static int test_bs_null(void) {
    TEST_START("NULL pointer handling");

    TEST_ASSERT(bs_read_bit(NULL) == -1, "bs_read_bit(NULL) returns -1");
    TEST_ASSERT(bs_read_bits(NULL, 8) == 0, "bs_read_bits(NULL, 8) returns 0");
    TEST_ASSERT(bs_eof(NULL) == 1, "bs_eof(NULL) returns 1");
    bs_done(NULL);  /* Should not crash */

    TEST_ASSERT(bsw_write_bit(NULL, 1) == 0, "bsw_write_bit(NULL, 1) does not crash");
    bsw_done(NULL);  /* Should not crash */

    TEST_END;
    return 0;
}

int main(void) {
    printf("=== Bitstream Test Suite ===\n\n");

    int failures = 0;

    failures += test_bs_new();
    failures += test_bs_read_bit();
    failures += test_bs_read_bits();
    failures += test_bs_eof();
    failures += test_bsw_write();
    failures += test_bsw_flush();
    failures += test_bs_null();

    printf("\n=== Results: %s test(s) ===", failures == 0 ? "ALL PASSED" : "SOME FAILED");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

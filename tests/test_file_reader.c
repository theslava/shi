/*
 *      test_file_reader.c
 *
 *      Tests for the file reader API.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_helpers.h"
#include "io/file_io.h"

/* Test: Create and destroy a file reader */
static int test_fr_new(void) {
    TEST_START("fr_new / fr_done");

    create_temp_file("test_fr.tmp", "hello");

    fr_fd* fd = fr_new("test_fr.tmp", 64);
    TEST_ASSERT(fd != NULL, "fr_new returns non-NULL");

    fr_done(fd);
    remove("test_fr.tmp");

    TEST_END;
    return 0;
}

/* Test: Read single byte */
static int test_fr_read(void) {
    TEST_START("fr_read");

    create_temp_file("test_fr_byte.tmp", "AB");

    fr_fd* fd = fr_new("test_fr_byte.tmp", 64);
    TEST_ASSERT(fd != NULL, "fr_new returns non-NULL");

    int byte1 = fr_read(fd);
    TEST_ASSERT(byte1 == 'A', "first byte is 'A'");

    int byte2 = fr_read(fd);
    TEST_ASSERT(byte2 == 'B', "second byte is 'B'");

    fr_done(fd);
    remove("test_fr_byte.tmp");

    TEST_END;
    return 0;
}

/* Test: Read all bytes */
static int test_fr_read_all(void) {
    TEST_START("fr_read all bytes");

    const char* content = "Hello!";
    create_temp_file("test_fr_all.tmp", content);

    fr_fd* fd = fr_new("test_fr_all.tmp", 64);
    TEST_ASSERT(fd != NULL, "fr_new returns non-NULL");

    size_t i = 0;
    int byte;
    while ((byte = fr_read(fd)) != EOF) {
        TEST_ASSERT(byte == content[i], "byte matches");
        i++;
    }
    TEST_ASSERT(i == strlen(content), "read correct number of bytes");

    fr_done(fd);
    remove("test_fr_all.tmp");

    TEST_END;
    return 0;
}

/* Test: Read with small buffer */
static int test_fr_buffer(void) {
    TEST_START("fr_read with small buffer");

    create_temp_file("test_fr_buf.tmp", "ABCDEFGHIJ");

    /* Use a 2-byte buffer — should still read everything */
    fr_fd* fd = fr_new("test_fr_buf.tmp", 2);
    TEST_ASSERT(fd != NULL, "fr_new with small buffer");

    int first = fr_read(fd);
    TEST_ASSERT(first == 'A', "first byte is 'A'");

    int last = fr_read(fd);
    TEST_ASSERT(last == 'B', "second byte is 'B'");

    fr_done(fd);
    remove("test_fr_buf.tmp");

    TEST_END;
    return 0;
}

/* Test: NULL pointer handling */
static int test_fr_null(void) {
    TEST_START("NULL pointer handling");

    fr_fd* fd = fr_new(NULL, 64);
    TEST_ASSERT(fd == NULL, "fr_new(NULL) returns NULL");

    fr_done(NULL); /* Should not crash */

    TEST_END;
    return 0;
}

int main(void) {
    printf("=== File Reader Test Suite ===\n\n");

    int failures = 0;

    failures += test_fr_new();
    failures += test_fr_read();
    failures += test_fr_read_all();
    failures += test_fr_buffer();
    failures += test_fr_null();

    printf("\n=== Results: %s test(s) ===", failures == 0 ? "ALL PASSED" : "SOME FAILED");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
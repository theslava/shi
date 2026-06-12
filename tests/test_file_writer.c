/*
 *      test_file_writer.c
 *
 *      Tests for the file writer API.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_helpers.h"
#include "io/file_io.h"

/* Test: Create and destroy a file writer */
static int test_fw_new(void) {
    TEST_START("fw_new / fw_done");

    fw_fd *fd = fw_new("test_fw.tmp", 4096);
    TEST_ASSERT(fd != NULL, "fw_new returns non-NULL");

    fw_done(fd);
    remove("test_fw.tmp");

    TEST_END;
    return 0;
}

/* Test: Write a single byte */
static int test_fw_write(void) {
    TEST_START("fw_write_byte");

    fw_fd *fd = fw_new("test_fw_byte.tmp", 4096);
    TEST_ASSERT(fd != NULL, "fw_new returns non-NULL");

    fw_write_byte(fd, 'X');
    fw_done(fd);

    /* Read back and verify */
    FILE *fp = fopen("test_fw_byte.tmp", "r");
    TEST_ASSERT(fp != NULL, "file was created");

    int byte = fgetc(fp);
    TEST_ASSERT(byte == 'X', "written byte is 'X'");

    fclose(fp);
    remove("test_fw_byte.tmp");

    TEST_END;
    return 0;
}

/* Test: Write multiple bytes */
static int test_fw_write_multiple(void) {
    TEST_START("fw_write_byte multiple");

    const char *content = "Test!";
    fw_fd *fd = fw_new("test_fw_multi.tmp", 4096);
    TEST_ASSERT(fd != NULL, "fw_new returns non-NULL");

    for (int i = 0; content[i]; i++) {
        fw_write_byte(fd, content[i]);
    }
    fw_done(fd);

    /* Read back and verify */
    FILE *fp = fopen("test_fw_multi.tmp", "r");
    TEST_ASSERT(fp != NULL, "file was created");

    size_t i = 0;
    int byte;
    while ((byte = fgetc(fp)) != EOF) {
        TEST_ASSERT(byte == content[i], "byte matches");
        i++;
    }
    TEST_ASSERT(i == strlen(content), "wrote correct number of bytes");

    fclose(fp);
    remove("test_fw_multi.tmp");

    TEST_END;
    return 0;
}

/* Test: NULL pointer handling */
static int test_fw_null(void) {
    TEST_START("NULL pointer handling");

    fw_done(NULL);  /* Should not crash */

    TEST_END;
    return 0;
}

int main(void) {
    printf("=== File Writer Test Suite ===\n\n");

    int failures = 0;

    failures += test_fw_new();
    failures += test_fw_write();
    failures += test_fw_write_multiple();
    failures += test_fw_null();

    printf("\n=== Results: %s test(s) ===", failures == 0 ? "ALL PASSED" : "SOME FAILED");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
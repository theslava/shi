/*
 *      test_helpers.h
 *
 *      Shared utilities for all test files.
 */

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <stdio.h>

/* Create a temporary file with given content */
FILE* create_temp_file(const char* name, const char* content);

/* Create a temporary binary file with given content */
FILE* create_bin_file(const char* name, const unsigned char* data, int size);

/* Compare two files for equality */
int files_equal(const char* file1, const char* file2);

/* Copy a file from src to dst (binary mode). Returns 0 on success, -1 on error. */
int copy_file(const char* src, const char* dst);

/* Read entire file into a buffer (returns size, or -1 on error) */
int read_file(const char* path, unsigned char* buf, int max_size);

/* Print a test result */
#define TEST_ASSERT(condition, msg)                                 \
    do {                                                            \
        if (!(condition)) {                                         \
            fprintf(stderr, "FAIL: %s (line %d)\n", msg, __LINE__); \
            return 1;                                               \
        }                                                           \
        printf("  PASS: %s\n", msg);                                \
    } while (0)

#define TEST_START(name) printf("Test: %s\n", name)
#define TEST_END printf("  Result: OK\n\n")

#endif /* TEST_HELPERS_H */
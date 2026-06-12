/*
 *      test_helpers.h
 *
 *      Shared utilities for all test files.
 */

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

/* Create a temporary file with given content */
FILE *create_temp_file(const char *name, const char *content);

/* Compare two files for equality */
int files_equal(const char *file1, const char *file2);

/* Print a test result */
#define TEST_ASSERT(condition, msg) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL: %s (line %d)\n", msg, __LINE__); \
        return 1; \
    } \
    printf("  PASS: %s\n", msg); \
} while (0)

#define TEST_START(name) printf("Test: %s\n", name)
#define TEST_END printf("  Result: OK\n\n")

#endif /* TEST_HELPERS_H */
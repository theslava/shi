/*
 *      test_helpers.h
 *
 *      Shared utilities for all test files.
 */

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Create a temporary file with given content */
static FILE *create_temp_file(const char *name, const char *content) {
    FILE *fp = fopen(name, "w");
    if (!fp) {
        fprintf(stderr, "Error: could not create temp file '%s'\n", name);
        return NULL;
    }
    fprintf(fp, "%s", content);
    fclose(fp);
    return fp;
}

/* Remove a file if it exists */
static void remove_file(const char *name) {
    remove(name);
}

/* Compare two files for equality */
static int files_equal(const char *file1, const char *file2) {
    FILE *f1 = fopen(file1, "r");
    FILE *f2 = fopen(file2, "r");

    if (!f1 || !f2) {
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return 0;
    }

    int ch1, ch2;
    int equal = 1;

    while (1) {
        ch1 = fgetc(f1);
        ch2 = fgetc(f2);

        if (ch1 != ch2) {
            equal = 0;
            break;
        }

        if (ch1 == EOF) break;
    }

    fclose(f1);
    fclose(f2);

    return equal;
}

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
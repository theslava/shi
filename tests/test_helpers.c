/*
 *      test_helpers.c
 *
 *      Shared test utilities.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_helpers.h"

/* Create a temporary file with given content */
FILE *create_temp_file(const char *name, const char *content) {
    FILE *fp = fopen(name, "w");
    if (!fp) {
        fprintf(stderr, "Error: could not create temp file '%s'\n", name);
        return NULL;
    }
    fprintf(fp, "%s", content);
    fclose(fp);
    return fp;
}

/* Compare two files for equality */
int files_equal(const char *file1, const char *file2) {
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

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
FILE* create_temp_file(const char* name, const char* content) {
    FILE* fp = fopen(name, "w");
    if (!fp) {
        fprintf(stderr, "Error: could not create temp file '%s'\n", name);
        return NULL;
    }
    fprintf(fp, "%s", content);
    fclose(fp);
    return fp;
}

/* Create a temporary binary file with given content */
FILE* create_bin_file(const char* name, const unsigned char* data, int size) {
    FILE* fp = fopen(name, "wb");
    if (!fp) {
        fprintf(stderr, "Error: could not create temp file '%s'\n", name);
        return NULL;
    }
    if (data && size > 0) {
        fwrite(data, 1, size, fp);
    }
    fclose(fp);
    return fp;
}

/* Compare two files for equality */
int files_equal(const char* file1, const char* file2) {
    FILE* f1 = fopen(file1, "r");
    FILE* f2 = fopen(file2, "r");

    if (!f1 || !f2) {
        if (f1)
            fclose(f1);
        if (f2)
            fclose(f2);
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

        if (ch1 == EOF)
            break;
    }

    fclose(f1);
    fclose(f2);

    return equal;
}

/* Read entire file into a buffer (returns size, or -1 on error) */
int read_file(const char* path, unsigned char* buf, int max_size) {
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

/* Copy a file from src to dst (binary mode). Returns 0 on success, -1 on error. */
int copy_file(const char* src, const char* dst) {
    FILE* fin = fopen(src, "rb");
    if (!fin)
        return -1;
    FILE* fout = fopen(dst, "wb");
    if (!fout) {
        fclose(fin);
        return -1;
    }
    unsigned char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fin)) > 0) {
        if (fwrite(buf, 1, n, fout) != n) {
            fclose(fin);
            fclose(fout);
            return -1;
        }
    }
    fclose(fin);
    fclose(fout);
    return 0;
}

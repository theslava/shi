/*
 *      advanced_usage.c
 *
 *      Demonstrates advanced features: custom buffer sizes,
 *      error handling, and batch processing.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core/compress.h"
#include "core/decompress.h"
#include "io/file_reader.h"
#include "io/file_writer.h"

/* Process a single file through compress -> decompress cycle */
static int process_file(const char *input, const char *output, int buffer_size) {
    /* Open input file with custom buffer size */
    fr_fd *fd = fr_new(input, buffer_size);
    if (!fd) {
        fprintf(stderr, "Error: could not open '%s' with buffer %d\n", input, buffer_size);
        return -1;
    }

    /* Open output file for writing */
    fw_fd *out = fw_new(output);
    if (!out) {
        fprintf(stderr, "Error: could not open '%s' for writing\n", output);
        fr_done(fd);
        return -1;
    }

    /* Perform compression */
    int result = compress_file(input, output);

    /* Cleanup */
    fr_done(fd);
    fw_done(out);

    return result;
}

/* Batch process multiple files */
static int batch_process(const char **files, int count, const char *prefix) {
    printf("Batch processing %d files with prefix '%s'...\n", count, prefix);

    for (int i = 0; i < count; i++) {
        char compressed[256];
        char decompressed[256];

        snprintf(compressed, sizeof(compressed), "%s_compressed_%d.txt", prefix, i);
        snprintf(decompressed, sizeof(decompressed), "%s_decompressed_%d.txt", prefix, i);

        printf("  [%d/%d] Processing '%s'...\n", i + 1, count, files[i]);

        if (process_file(files[i], compressed, 128) == 0) {
            printf("    -> Compressed to '%s'\n", compressed);
            if (decompress_file(compressed, decompressed) == 0) {
                printf("    -> Decompressed to '%s'\n", decompressed);
            }
        }
    }

    return 0;
}

int main(void) {
    /* Create sample files for batch processing */
    const char *samples[] = {"sample1.txt", "sample2.txt", "sample3.txt"};
    const char *prefix = "batch";

    for (int i = 0; i < 3; i++) {
        FILE *fp = fopen(samples[i], "w");
        if (fp) {
            fprintf(fp, "Sample file %d: This is test data for batch processing.\n", i + 1);
            fclose(fp);
        }
    }

    /* Run batch processing */
    batch_process(samples, 3, prefix);

    printf("Batch processing complete.\n");

    return EXIT_SUCCESS;
}
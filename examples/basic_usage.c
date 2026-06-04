/*
 *      basic_usage.c
 *
 *      Demonstrates basic compression and decompression usage.
 *
 *      Compile: gcc -Wall -Wextra -std=c99 -Iinclude -o basic_usage basic_usage.c src/core/compress.c src/core/decompress.c src/io/file_reader.c src/io/file_writer.c src/data_structures/*.c src/utils/*.c
 *      Run:     ./basic_usage
 */

#include <stdio.h>
#include <stdlib.h>
#include "core/compress.h"
#include "core/decompress.h"

int main(void) {
    const char *input_file = "sample_input.txt";
    const char *compressed_file = "sample_output.txt";
    const char *decompressed_file = "sample_restored.txt";

    /* Step 1: Create a sample input file */
    printf("Creating sample input file...\n");
    FILE *fp = fopen(input_file, "w");
    if (!fp) {
        fprintf(stderr, "Error: could not create '%s'\n", input_file);
        return EXIT_FAILURE;
    }
    fprintf(fp, "Hello, world! This is a sample text for compression testing.\n");
    fprintf(fp, "Repetition is good for compression: hello hello hello.\n");
    fclose(fp);

    /* Step 2: Compress the file */
    printf("Compressing '%s' to '%s'...\n", input_file, compressed_file);
    if (compress_file(input_file, compressed_file) != 0) {
        fprintf(stderr, "Error: compression failed\n");
        return EXIT_FAILURE;
    }

    /* Step 3: Decompress the file */
    printf("Decompressing '%s' to '%s'...\n", compressed_file, decompressed_file);
    if (decompress_file(compressed_file, decompressed_file) != 0) {
        fprintf(stderr, "Error: decompression failed\n");
        return EXIT_FAILURE;
    }

    printf("Done! Check '%s' for the decompressed output.\n", decompressed_file);

    return EXIT_SUCCESS;
}
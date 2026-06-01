/*
 *      test_bitstream.c
 *
 *      Demonstration of the bitstream API.
 *      Reads test.txt byte-by-byte and prints each bit sequentially.
 *
 *      Compile: gcc -Wall -Wextra -std=c99 -Iinclude -o test_bitstream test_bitstream.c file_reader.c bitstream.c
 *      Run:     ./test_bitstream test.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_reader.h"
#include "bitstream.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Open the file with a 64-byte buffer for demo purposes */
    fr_fd *fd = fr_new(argv[1], 64);
    if (!fd) {
        fprintf(stderr, "Error: could not open '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }

    /* Create a bitstream wrapping the file descriptor */
    bitstream *bs = bs_new(fd);
    if (!bs) {
        fprintf(stderr, "Error: could not create bitstream\n");
        fr_done(fd);
        return EXIT_FAILURE;
    }

    printf("=== Sequential bit reading demo ===\n");
    printf("File: %s\n\n", argv[1]);

    /* Read and print each bit one at a time */
    int bit_count = 0;
    int byte_count = 0;
    while (!bs_eof(bs)) {
        int bit = bs_read_bit(bs);
        if (bit == -1) break;  /* EOF */

        printf("%d", bit);
        bit_count++;

        /* Print a space after every 8 bits (one byte's worth) */
        if (bit_count % 8 == 0) {
            byte_count++;
            printf("  (byte %d)", byte_count);
            if (bit_count % 64 == 0) {
                printf("\n");  /* Newline every 8 bytes */
            }
        }
    }

    printf("\n\nTotal bits read: %d  |  Total bytes: %d\n", bit_count, byte_count);

    /* Cleanup */
    bs_done(bs);
    fr_done(fd);

    return EXIT_SUCCESS;
}
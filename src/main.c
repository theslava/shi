/* Released to the public domain — No rights reserved. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core/compress.h"
#include "core/decompress.h"

static void print_usage(const char* prog) {
    fprintf(stderr, "Usage: %s [options] <compress|decompress> <input_file> <output_file>\n", prog);
    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, "  -v, --verbose    Enable verbose output\n");
    fprintf(stderr, "  -h, --help       Show this help message\n");
    fprintf(stderr, "\nExamples:\n");
    fprintf(stderr, "  %s compress input.txt output.txt\n", prog);
    fprintf(stderr, "  %s decompress input.txt output.txt\n", prog);
    fprintf(stderr, "  %s -v compress input.bin output.huf\n", prog);
}

int main(int argc, char* argv[]) {
    int verbose = 0;
    int arg_start = 1;

    /* Parse optional flags */
    while (arg_start < argc) {
        if ((strcmp(argv[arg_start], "-v") == 0 || strcmp(argv[arg_start], "--verbose") == 0)) {
            verbose = 1;
            arg_start++;
        } else if (strcmp(argv[arg_start], "-h") == 0 || strcmp(argv[arg_start], "--help") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        } else {
            break;
        }
    }

    set_verbose(verbose);

    if (argc - arg_start != 3) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char* command = argv[arg_start];
    const char* input_file = argv[arg_start + 1];
    const char* output_file = argv[arg_start + 2];

    if (strcmp(command, "compress") == 0) {
        printf("Compressing '%s' to '%s'\n", input_file, output_file);
        return compress_file(input_file, output_file) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    } else if (strcmp(command, "decompress") == 0) {
        printf("Decompressing '%s' to '%s'\n", input_file, output_file);
        return decompress_file(input_file, output_file) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    } else {
        fprintf(stderr, "Error: Unknown command '%s'. Use 'compress' or 'decompress'.\n", command);
        return EXIT_FAILURE;
    }
}

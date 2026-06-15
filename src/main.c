/* Released to the public domain — No rights reserved. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core/compress.h"
#include "core/decompress.h"
#include "core/version.h"

static void print_usage(const char* prog) {
    fprintf(stderr, "Usage: %s [--version <N>] <compress|decompress> <input_file> <output_file>\n",
            prog);
    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, "  --version <N>    Compress/decompress using format version N (default: %d)\n",
            SHI_CURRENT_VERSION);
    fprintf(stderr, "  -v, --verbose    Enable verbose output\n");
    fprintf(stderr, "  -h, --help       Show this help message\n");
    fprintf(stderr, "\nExamples:\n");
    fprintf(stderr, "  %s compress input.txt output.txt\n", prog);
    fprintf(stderr, "  %s decompress input.txt output.txt\n", prog);
    fprintf(stderr, "  %s --version 0 compress input.bin output.huf\n", prog);
    fprintf(stderr, "  %s -v compress input.bin output.huf\n", prog);
}

int main(int argc, char* argv[]) {
    int verbose = 0;
    int version = SHI_CURRENT_VERSION;
    int arg_start = 1;

    /* Parse optional flags and --version */
    while (arg_start < argc) {
        if (strcmp(argv[arg_start], "--version") == 0) {
            /* Need a version number argument */
            if (arg_start + 1 >= argc) {
                fprintf(stderr, "Error: --version requires a numeric argument.\n");
                return EXIT_FAILURE;
            }
            version = atoi(argv[arg_start + 1]);
            if (version < 0 || version > SHI_MAX_VERSION) {
                fprintf(stderr, "Error: Unsupported version %d. Supported: 0-%d.\n", version,
                        SHI_MAX_VERSION);
                return EXIT_FAILURE;
            }
            arg_start += 2;
        } else if (strcmp(argv[arg_start], "-v") == 0 ||
                   strcmp(argv[arg_start], "--verbose") == 0) {
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
        printf("Compressing '%s' to '%s' (version %d)\n", input_file, output_file, version);

        switch (version) {
        case 0:
            return shi_compress_v0(input_file, output_file) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
        default:
            fprintf(stderr, "Error: Compression not supported for version %d.\n", version);
            return EXIT_FAILURE;
        }
    } else if (strcmp(command, "decompress") == 0) {
        printf("Decompressing '%s' to '%s'\n", input_file, output_file);

        /* Detect version from the 4th magic byte */
        fr_fd* input_fd = fr_new((char*)input_file, 4096);
        if (!input_fd) {
            fprintf(stderr, "Error: Could not open input file '%s'\n", input_file);
            return EXIT_FAILURE;
        }

        /* Read first 4 bytes to check magic */
        unsigned char magic[4];
        for (int i = 0; i < 4; i++) {
            int b = fr_read(input_fd);
            if (b == EOF) {
                fprintf(stderr, "Error: File '%s' too small to read magic bytes.\n", input_file);
                fr_done(input_fd);
                return EXIT_FAILURE;
            }
            magic[i] = (unsigned char)b;
        }

        int detected_version = -1;
        if (magic[0] == 0x53 && magic[1] == 0x48 && magic[2] == 0x49) {
            detected_version = magic[3];
        }

        fr_done(input_fd);

        if (detected_version < 0) {
            fprintf(stderr, "Error: Invalid file format - bad magic bytes in '%s'.\n", input_file);
            return EXIT_FAILURE;
        }

        switch (detected_version) {
        case 0:
            return shi_decompress_v0(input_file, output_file) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
        default:
            fprintf(stderr, "Error: Decompression not supported for version %d.\n",
                    detected_version);
            return EXIT_FAILURE;
        }
    } else {
        fprintf(stderr, "Error: Unknown command '%s'. Use 'compress' or 'decompress'.\n", command);
        return EXIT_FAILURE;
    }
}

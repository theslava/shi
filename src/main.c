/* Released to the public domain — No rights reserved. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli/args.h"
#include "core/compress.h"
#include "core/decompress.h"
#include "core/version.h"

int main(int argc, const char* argv[]) {
    shi_args_t args;
    shi_args_result_t result = shi_parse_args(argc, argv, &args);

    switch (result) {
    case ARGS_ERR_HELP:
        shi_print_usage(argv[0]);
        return EXIT_SUCCESS;
    case ARGS_OK:
        break;
    case ARGS_ERR_BAD_USAGE:
        shi_print_usage(argv[0]);
        return EXIT_FAILURE;
    default:
        return EXIT_FAILURE;
    }

    set_verbose(args.verbose);

    /* Dispatch based on parsed command */
    switch (args.command) {
    case CMD_COMPRESS:
        printf("Compressing '%s' to '%s' (version %d)\n", args.input_file, args.output_file,
               args.version);
        return shi_compress_v0(args.input_file, args.output_file) == 0 ? EXIT_SUCCESS
                                                                       : EXIT_FAILURE;

    case CMD_DECOMPRESS:
        printf("Decompressing '%s' to '%s'\n", args.input_file, args.output_file);

        /* Detect version from the 4th magic byte */
        fr_fd* input_fd = fr_new((char*)args.input_file, 4096);
        if (!input_fd) {
            fprintf(stderr, "Error: Could not open input file '%s'\n", args.input_file);
            return EXIT_FAILURE;
        }

        unsigned char magic[4];
        for (int i = 0; i < 4; i++) {
            int b = fr_read(input_fd);
            if (b == EOF) {
                fprintf(stderr, "Error: File '%s' too small to read magic bytes.\n",
                        args.input_file);
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
            fprintf(stderr, "Error: Invalid file format - bad magic bytes in '%s'.\n",
                    args.input_file);
            return EXIT_FAILURE;
        }

        switch (detected_version) {
        case 0:
            return shi_decompress_v0(args.input_file, args.output_file) == 0 ? EXIT_SUCCESS
                                                                             : EXIT_FAILURE;
        default:
            fprintf(stderr, "Error: Decompression not supported for version %d.\n",
                    detected_version);
            return EXIT_FAILURE;
        }

    case CMD_NONE:
        /* Should not happen — parse_args validates command */
        shi_print_usage(argv[0]);
        return EXIT_FAILURE;
    }
}

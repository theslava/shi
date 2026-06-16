/* Released to the public domain — No rights reserved. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli/args.h"
#include "core/compress.h"
#include "core/decompress.h"
#include "core/version.h"

/* ---------------------------------------------------------------------------
 * Internal helper: derive the default output path from input file and command.
 *
 * For compression: appends SHI_COMPRESSED_EXT (".shi") to the input path.
 * For decompression: strips SHI_COMPRESSED_EXT from the input path.
 *
 * The caller must free the returned pointer.
 * Returns NULL on allocation failure.
 * --------------------------------------------------------------------------- */
static char* derive_default_output(const char* input_file, shi_command_t command) {
    /* Determine the length needed: strlen(input) + strlen(ext) + 1 (null) */
    size_t input_len = strlen(input_file);
    size_t ext_len = strlen(SHI_COMPRESSED_EXT);
    char* result = (char*)malloc(input_len + ext_len + 1);
    if (!result)
        return NULL;

    if (command == CMD_COMPRESS) {
        /* Append extension: "input.txt" -> "input.txt.shi" */
        strcpy(result, input_file);
        strcat(result, SHI_COMPRESSED_EXT);
    } else {
        /* Strip extension: "input.shi" -> "input" */
        /* Find the last '.' in the filename */
        const char* dot = strrchr(input_file, '.');
        if (dot && (size_t)(dot - input_file + ext_len + 1) == input_len + 1) {
            /* The suffix after '.' matches the extension exactly */
            size_t base_len = (size_t)(dot - input_file);
            memcpy(result, input_file, base_len);
            result[base_len] = '\0';
        } else {
            /* Fallback: just copy (shouldn't happen since parser validated .shi) */
            strcpy(result, input_file);
        }
    }

    return result;
}

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
        fprintf(stderr, "%s\n", shi_args_error_msg(result));
        return EXIT_FAILURE;
    }

    set_verbose(args.verbose);

    /* Derive output file from input file */
    char* output_file = derive_default_output(args.input_file, args.command);
    if (!output_file) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return EXIT_FAILURE;
    }

    /* Dispatch based on parsed command */
    switch (args.command) {
    case CMD_COMPRESS: {
        printf("Compressing '%s' to '%s' (version %d)\n", args.input_file, output_file,
               args.version);
        const int rc = shi_compress_v0(args.input_file, output_file);
        free(output_file);
        return rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    case CMD_DECOMPRESS: {
        printf("Decompressing '%s' to '%s'\n", args.input_file, output_file);

        /* Detect version from the 4th magic byte */
        fr_fd* input_fd = fr_new((char*)args.input_file, 4096);
        if (!input_fd) {
            fprintf(stderr, "Error: Could not open input file '%s'\n", args.input_file);
            free(output_file);
            return EXIT_FAILURE;
        }

        unsigned char magic[4];
        for (int i = 0; i < 4; i++) {
            int b = fr_read(input_fd);
            if (b == EOF) {
                fprintf(stderr, "Error: File '%s' too small to read magic bytes.\n",
                        args.input_file);
                fr_done(input_fd);
                free(output_file);
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
            free(output_file);
            return EXIT_FAILURE;
        }

        switch (detected_version) {
        case 0: {
            const int rc = shi_decompress_v0(args.input_file, output_file);
            free(output_file);
            return rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
        }
        default:
            fprintf(stderr, "Error: Decompression not supported for version %d.\n",
                    detected_version);
            free(output_file);
            return EXIT_FAILURE;
        }
    }

    case CMD_NONE:
        /* Should not happen — parse_args validates command */
        shi_print_usage(argv[0]);
        free(output_file);
        return EXIT_FAILURE;
    }
}

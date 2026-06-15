/*
 *      args.c
 *
 *      CLI argument parsing for the shi Huffman compression tool.
 *      Zero-dependency manual parser supporting both short (-v) and
 *      long (--verbose) option forms, combined short flags (-vh),
 *      and -- to stop flag parsing.
 *
 *      Released to the public domain — No rights reserved.
 */

#include "cli/args.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/version.h"

/* ---------------------------------------------------------------------------
 * Internal helper: check if a string starts with a given prefix.
 * --------------------------------------------------------------------------- */
static int starts_with(const char* str, const char* prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

/* ---------------------------------------------------------------------------
 * Internal helper: parse a single short option character.
 * Returns ARGS_OK on success, ARGS_ERR_HELP if -h/--help, or
 * ARGS_ERR_UNKNOWN for unrecognized flags.
 * --------------------------------------------------------------------------- */
static shi_args_result_t parse_short_opt(char opt, int* verbose, shi_args_result_t* help_flag) {
    switch (opt) {
    case 'v':
        *verbose = 1;
        break;
    case 'h':
        *help_flag = ARGS_ERR_HELP;
        break;
    default:
        fprintf(stderr, "Error: Unknown option '-%c'.\n", opt);
        return ARGS_ERR_UNKNOWN;
    }
    return ARGS_OK;
}

/* ---------------------------------------------------------------------------
 * Internal helper: parse a --version argument.
 * --------------------------------------------------------------------------- */
static shi_args_result_t parse_version_arg(const char* arg, int* version) {
    /* Must be a non-empty string of digits */
    if (arg[0] < '0' || arg[0] > '9') {
        fprintf(stderr, "Error: --version requires a non-negative integer argument.\n");
        return ARGS_ERR_BAD_ARG;
    }
    for (int i = 1; arg[i]; i++) {
        if (arg[i] < '0' || arg[i] > '9') {
            fprintf(stderr, "Error: --version requires a non-negative integer argument.\n");
            return ARGS_ERR_BAD_ARG;
        }
    }

    long val = strtol(arg, NULL, 10);
    if (val < 0 || val > SHI_MAX_VERSION) {
        fprintf(stderr, "Error: Unsupported version %ld. Supported: 0-%d.\n", val, SHI_MAX_VERSION);
        return ARGS_ERR_BAD_ARG;
    }
    *version = (int)val;
    return ARGS_OK;
}

/* ---------------------------------------------------------------------------
 * Public API
 * --------------------------------------------------------------------------- */

void shi_print_usage(const char* prog) {
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

shi_args_result_t shi_parse_args(int argc, const char* argv[], shi_args_t* out) {
    if (!out || argc <= 0 || !argv) {
        return ARGS_ERR_BAD_USAGE;
    }

    /* Initialize defaults */
    out->verbose = 0;
    out->version = SHI_CURRENT_VERSION;
    out->command = CMD_NONE;
    out->input_file = NULL;
    out->output_file = NULL;

    int verbose = 0;
    int version = SHI_CURRENT_VERSION;
    shi_args_result_t help_flag = ARGS_OK;
    int arg_start = 1;

    /* Pass 1: parse flags */
    while (arg_start < argc) {
        const char* arg = argv[arg_start];

        /* -- stops flag parsing */
        if (strcmp(arg, "--") == 0) {
            arg_start++;
            break;
        }

        /* Long options */
        if (starts_with(arg, "--")) {
            if (strcmp(arg, "--version") == 0) {
                if (arg_start + 1 >= argc) {
                    fprintf(stderr, "Error: --version requires a numeric argument.\n");
                    return ARGS_ERR_BAD_ARG;
                }
                shi_args_result_t res = parse_version_arg(argv[arg_start + 1], &version);
                if (res != ARGS_OK) {
                    return res;
                }
                arg_start += 2;
            } else if (strcmp(arg, "--verbose") == 0) {
                verbose = 1;
                arg_start++;
            } else if (strcmp(arg, "--help") == 0) {
                help_flag = ARGS_ERR_HELP;
                arg_start++;
            } else {
                fprintf(stderr, "Error: Unknown option '%s'.\n", arg);
                return ARGS_ERR_UNKNOWN;
            }
            continue;
        }

        /* Short options (single dash) */
        if (arg[0] == '-' && arg[1] != '\0') {
            /* Support combined short flags: -vh => -v then -h */
            for (int i = 1; arg[i] != '\0'; i++) {
                shi_args_result_t res = parse_short_opt(arg[i], &verbose, &help_flag);
                if (res != ARGS_OK) {
                    return res;
                }
            }
            arg_start++;
            continue;
        }

        /* Not a flag — stop parsing flags */
        break;
    }

    /* If -h/--help was seen, store parsed values and return HELP */
    if (help_flag == ARGS_ERR_HELP) {
        out->verbose = verbose;
        out->version = version;
        out->command = CMD_NONE;
        out->input_file = NULL;
        out->output_file = NULL;
        return ARGS_ERR_HELP;
    }

    /* Pass 2: validate positional arguments */
    int remaining = argc - arg_start;
    if (remaining != 3) {
        return ARGS_ERR_BAD_USAGE;
    }

    out->verbose = verbose;
    out->version = version;
    out->command = CMD_NONE;
    out->input_file = argv[arg_start + 1];
    out->output_file = argv[arg_start + 2];

    /* Validate command */
    if (strcmp(argv[arg_start], "compress") == 0) {
        out->command = CMD_COMPRESS;
    } else if (strcmp(argv[arg_start], "decompress") == 0) {
        out->command = CMD_DECOMPRESS;
    } else {
        fprintf(stderr, "Error: Unknown command '%s'. Use 'compress' or 'decompress'.\n",
                argv[arg_start]);
        return ARGS_ERR_BAD_CMD;
    }

    return ARGS_OK;
}

const char* shi_args_error_msg(shi_args_result_t result) {
    switch (result) {
    case ARGS_OK:
        return "No error";
    case ARGS_ERR_NO_MEMORY:
        return "Internal error: out of memory";
    case ARGS_ERR_BAD_ARG:
        return "Error: Invalid argument value";
    case ARGS_ERR_BAD_CMD:
        return "Error: Unknown command";
    case ARGS_ERR_BAD_USAGE:
        return "Error: Incorrect usage";
    case ARGS_ERR_HELP:
        return "Help requested";
    case ARGS_ERR_UNKNOWN:
        return "Error: Unknown option";
    default:
        return "Unknown error";
    }
}

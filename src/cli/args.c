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
static shi_args_result_t parse_short_opt(
    char opt, int* verbose, shi_command_t* command, shi_args_result_t* help_flag, int* has_mode) {
    (void)verbose;
    (void)command;
    (void)help_flag;
    (void)has_mode;
    switch (opt) {
    case 'c':
        if (*has_mode) {
            fprintf(stderr, "Error: Cannot use both -c and -d.\n");
            return ARGS_ERR_BAD_ARG;
        }
        *command = CMD_COMPRESS;
        *has_mode = 1;
        break;
    case 'd':
        if (*has_mode) {
            fprintf(stderr, "Error: Cannot use both -c and -d.\n");
            return ARGS_ERR_BAD_ARG;
        }
        *command = CMD_DECOMPRESS;
        *has_mode = 1;
        break;
    case 'f':
        /* -f takes an argument — handled by the caller, not here */
        break;
    case 'v':
        *verbose = 1;
        break;
    case 'V':
        /* -V is handled by the caller as --version */
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
    fprintf(stderr, "Usage: %s [options] -f <input_file>\n", prog);
    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, "  -c, --compress     Compress mode\n");
    fprintf(stderr, "  -d, --decompress   Decompress mode\n");
    fprintf(stderr, "  -f, --file <path>  Input file path (required)\n");
    fprintf(stderr, "  -v, --verbose      Enable verbose output\n");
    fprintf(stderr, "  -V, --version <N>  Format version N (default: %d)\n", SHI_CURRENT_VERSION);
    fprintf(stderr, "  -h, --help         Show this help message\n");
    fprintf(stderr, "\nExamples:\n");
    fprintf(stderr, "  %s -c -f input.txt              # outputs input.txt.shi\n", prog);
    fprintf(stderr, "  %s -d -f input.shi              # outputs input\n", prog);
    fprintf(stderr, "  %s -c -f input.bin --verbose    # outputs input.bin.shi\n", prog);
    fprintf(stderr, "  %s --version 0 -d -f input.shi  # outputs input\n", prog);
}

shi_args_result_t shi_parse_args(int argc, const char* argv[], shi_args_t* out) {
    if (!out || argc <= 0 || !argv) {
        return ARGS_ERR_BAD_USAGE;
    }

    /* Initialize defaults */
    out->command = CMD_NONE;
    out->verbose = 0;
    out->version = SHI_CURRENT_VERSION;
    out->input_file = NULL;

    int verbose = 0;
    int version = SHI_CURRENT_VERSION;
    shi_command_t command = CMD_NONE;
    shi_args_result_t help_flag = ARGS_OK;
    const char* input_file = NULL;
    int has_mode = 0;
    int has_file = 0;

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
            if (strcmp(arg, "--compress") == 0) {
                if (has_mode) {
                    fprintf(stderr, "Error: Cannot use both --compress and --decompress.\n");
                    return ARGS_ERR_BAD_ARG;
                }
                command = CMD_COMPRESS;
                has_mode = 1;
                arg_start++;
            } else if (strcmp(arg, "--decompress") == 0) {
                if (has_mode) {
                    fprintf(stderr, "Error: Cannot use both --compress and --decompress.\n");
                    return ARGS_ERR_BAD_ARG;
                }
                command = CMD_DECOMPRESS;
                has_mode = 1;
                arg_start++;
            } else if (strcmp(arg, "--file") == 0) {
                if (arg_start + 1 >= argc) {
                    fprintf(stderr, "Error: --file requires a file path argument.\n");
                    return ARGS_ERR_BAD_ARG;
                }
                input_file = argv[arg_start + 1];
                has_file = 1;
                arg_start += 2;
            } else if (strcmp(arg, "--verbose") == 0) {
                verbose = 1;
                arg_start++;
            } else if (strcmp(arg, "--version") == 0) {
                if (arg_start + 1 >= argc) {
                    fprintf(stderr, "Error: --version requires a numeric argument.\n");
                    return ARGS_ERR_BAD_ARG;
                }
                shi_args_result_t res = parse_version_arg(argv[arg_start + 1], &version);
                if (res != ARGS_OK) {
                    return res;
                }
                arg_start += 2;
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
                shi_args_result_t res =
                    parse_short_opt(arg[i], &verbose, &command, &help_flag, &has_mode);
                if (res == ARGS_OK && arg[i] == 'f') {
                    /* -f requires a following argument */
                    if (arg[i + 1] != '\0') {
                        /* -fvalue form: -fin.txt */
                        input_file = &arg[i + 1];
                        has_file = 1;
                    } else if (arg_start + 1 < argc) {
                        /* -f value form: -f in.txt */
                        input_file = argv[arg_start + 1];
                        has_file = 1;
                        arg_start++;
                    } else {
                        fprintf(stderr, "Error: -f requires a file path argument.\n");
                        return ARGS_ERR_BAD_ARG;
                    }
                }
                if (res == ARGS_OK && arg[i] == 'V') {
                    /* -V requires a following argument */
                    if (arg[i + 1] != '\0') {
                        /* -Vvalue form: -V0 */
                        shi_args_result_t vres = parse_version_arg(&arg[i + 1], &version);
                        if (vres != ARGS_OK) {
                            return vres;
                        }
                    } else if (arg_start + 1 < argc) {
                        /* -V value form: -V 0 */
                        shi_args_result_t vres = parse_version_arg(argv[arg_start + 1], &version);
                        if (vres != ARGS_OK) {
                            return vres;
                        }
                        arg_start++;
                    } else {
                        fprintf(stderr, "Error: --version requires a numeric argument.\n");
                        return ARGS_ERR_BAD_ARG;
                    }
                }
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
        return ARGS_ERR_HELP;
    }

    /* Validate required arguments */
    if (!has_mode) {
        fprintf(stderr, "Error: Must specify -c/--compress or -d/--decompress.\n");
        return ARGS_ERR_BAD_USAGE;
    }

    if (!has_file) {
        fprintf(stderr, "Error: Must specify -f/--file with an input file path.\n");
        return ARGS_ERR_BAD_USAGE;
    }

    if (!input_file) {
        fprintf(stderr, "Error: --file requires a file path argument.\n");
        return ARGS_ERR_BAD_ARG;
    }

    /* Validate .shi extension for decompression input */
    if (command == CMD_DECOMPRESS) {
        const char* ext = strrchr(input_file, '.');
        if (!ext || strcmp(ext, SHI_COMPRESSED_EXT) != 0) {
            fprintf(stderr, "Error: Decompression input must have '%s' extension.\n",
                    SHI_COMPRESSED_EXT);
            return ARGS_ERR_BAD_ARG;
        }
    }

    /* Store parsed values */
    out->verbose = verbose;
    out->version = version;
    out->command = command;
    out->input_file = input_file;

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

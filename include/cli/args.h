/*
 *      args.h
 *
 *      CLI argument parsing for the shi Huffman compression tool.
 *      Standalone, zero-dependency manual parser supporting both
 *      short (-v) and long (--verbose) option forms.
 *
 *      Released to the public domain — No rights reserved.
 */

#ifndef __args_h__
#define __args_h__

#include <stddef.h>

/* ---------------------------------------------------------------------------
 * Parsed argument result
 * --------------------------------------------------------------------------- */

/* Commands supported by the tool */
typedef enum { CMD_NONE = 0, CMD_COMPRESS, CMD_DECOMPRESS } shi_command_t;

/* Parsing result codes */
typedef enum {
    ARGS_OK = 0,        /* All parsed successfully */
    ARGS_ERR_NO_MEMORY, /* Internal error (should not happen) */
    ARGS_ERR_BAD_ARG,   /* Invalid argument value (e.g. bad version number) */
    ARGS_ERR_BAD_CMD,   /* Unknown command */
    ARGS_ERR_BAD_USAGE, /* Wrong number of positional arguments */
    ARGS_ERR_HELP,      /* --help/-h was requested */
    ARGS_ERR_UNKNOWN    /* Unrecognized flag */
} shi_args_result_t;

/* Parsed CLI arguments */
typedef struct {
    int verbose;             /* 1 if -v/--verbose was set */
    int version;             /* Format version (default: SHI_CURRENT_VERSION) */
    shi_command_t command;   /* Compress or decompress */
    const char* input_file;  /* Positional: input file path */
    const char* output_file; /* Positional: output file path */
} shi_args_t;

/* ---------------------------------------------------------------------------
 * API
 * --------------------------------------------------------------------------- */

/**
 * Print usage/help text to stderr.
 * @param prog  Program name (argv[0]), used for the usage line.
 */
void shi_print_usage(const char* prog);

/**
 * Parse command-line arguments.
 *
 * Supported flags (must appear before positional args):
 *   --version <N>    Format version (default: SHI_CURRENT_VERSION)
 *   -v, --verbose    Enable verbose output
 *   -h, --help       Show help and exit
 *   --               Stop flag parsing (flags after -- are treated as positional)
 *
 * Combined short flags are supported: -vh is equivalent to -v -h.
 *
 * @param argc  Argument count (from main).
 * @param argv  Argument vector (from main).
 * @param out   Output struct to fill on success.
 * @return      ARGS_OK on success, or an error code.
 *              On ARGS_ERR_HELP, the caller should print usage and exit 0.
 *              On other errors, the caller should print the error message
 *              (via shi_args_error_msg()) and exit 1.
 */
shi_args_result_t shi_parse_args(int argc, const char* argv[], shi_args_t* out);

/**
 * Get a human-readable error message for a result code.
 * @param result  The result code from shi_parse_args().
 * @return        Static string describing the error.
 */
const char* shi_args_error_msg(shi_args_result_t result);

#endif /* __args_h__ */

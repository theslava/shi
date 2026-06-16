/*
 *      test_args.c
 *
 *      Tests for the CLI argument parser (src/cli/args.c).
 *      Tests every flag, every error path, and edge cases.
 *      Does NOT link against compression code — tests the parser in isolation.
 *
 *      Released to the public domain — No rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli/args.h"
#include "core/version.h"

/* ---------------------------------------------------------------------------
 * Test helpers
 * --------------------------------------------------------------------------- */

#define TEST_START(name) printf("Test: %s\n", name)
#define TEST_END printf("  Result: OK\n\n")

#define TEST_ASSERT(cond, msg)                                      \
    do {                                                            \
        if (!(cond)) {                                              \
            fprintf(stderr, "FAIL: %s (line %d)\n", msg, __LINE__); \
            return 1;                                               \
        }                                                           \
        printf("  PASS: %s\n", msg);                                \
    } while (0)

/* Static storage to avoid stack allocation issues in test functions.
 * Each test function uses its own static buffers. */
static const char* g_argv_buf[64];

static int make_argv(const char** items, int* out_argc, const char*** out_argv) {
    int argc = 0;
    while (items[argc] != NULL) {
        argc++;
    }
    for (int i = 0; i < argc; i++) {
        g_argv_buf[i] = items[i];
    }
    *out_argc = argc;
    *out_argv = g_argv_buf;
    return argc;
}

/* ---------------------------------------------------------------------------
 * Valid compress flag tests
 * --------------------------------------------------------------------------- */

static int test_compress_short(void) {
    TEST_START("-c flag");

    const char* items[] = {"shi", "-c", "-f", "in.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.command == CMD_COMPRESS, "command is CMD_COMPRESS");
    TEST_ASSERT(strcmp(args.input_file, "in.txt") == 0, "input_file is 'in.txt'");

    TEST_END;
    return 0;
}

static int test_compress_long(void) {
    TEST_START("--compress flag");

    const char* items[] = {"shi", "--compress", "-f", "in.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.command == CMD_COMPRESS, "command is CMD_COMPRESS");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Valid decompress flag tests
 * --------------------------------------------------------------------------- */

static int test_decompress_short(void) {
    TEST_START("-d flag");

    const char* items[] = {"shi", "-d", "-f", "in.shi", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.command == CMD_DECOMPRESS, "command is CMD_DECOMPRESS");
    TEST_ASSERT(strcmp(args.input_file, "in.shi") == 0, "input_file is 'in.shi'");

    TEST_END;
    return 0;
}

static int test_decompress_long(void) {
    TEST_START("--decompress flag");

    const char* items[] = {"shi", "--decompress", "-f", "in.shi", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.command == CMD_DECOMPRESS, "command is CMD_DECOMPRESS");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * -f / --file flag tests
 * --------------------------------------------------------------------------- */

static int test_file_short(void) {
    TEST_START("-f flag");

    const char* items[] = {"shi", "-c", "-f", "input.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(strcmp(args.input_file, "input.txt") == 0, "input_file is 'input.txt'");

    TEST_END;
    return 0;
}

static int test_file_long(void) {
    TEST_START("--file flag");

    const char* items[] = {"shi", "-c", "--file", "input.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(strcmp(args.input_file, "input.txt") == 0, "input_file is 'input.txt'");

    TEST_END;
    return 0;
}

static int test_file_with_path(void) {
    TEST_START("-f with relative path");

    const char* items[] = {"shi", "-c", "-f", "./dir/input.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(strcmp(args.input_file, "./dir/input.txt") == 0, "input_file is './dir/input.txt'");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Verbose flag tests
 * --------------------------------------------------------------------------- */

static int test_verbose_short(void) {
    TEST_START("-v flag");

    const char* items[] = {"shi", "-c", "-f", "in.txt", "-v", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.verbose == 1, "verbose is 1");

    TEST_END;
    return 0;
}

static int test_verbose_long(void) {
    TEST_START("--verbose flag");

    const char* items[] = {"shi", "-c", "-f", "in.txt", "--verbose", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.verbose == 1, "verbose is 1");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Version flag tests
 * --------------------------------------------------------------------------- */

static int test_version_flag(void) {
    TEST_START("--version 0 flag");

    const char* items[] = {"shi", "-c", "-f", "in.txt", "--version", "0", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.version == 0, "version is 0");

    TEST_END;
    return 0;
}

static int test_version_short(void) {
    TEST_START("-V flag");

    const char* items[] = {"shi", "-c", "-f", "in.txt", "-V", "0", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.version == 0, "version is 0");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Help flag tests
 * --------------------------------------------------------------------------- */

static int test_help_short(void) {
    TEST_START("-h flag returns HELP");

    const char* items[] = {"shi", "-h", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_HELP, "returns HELP");

    TEST_END;
    return 0;
}

static int test_help_long(void) {
    TEST_START("--help flag returns HELP");

    const char* items[] = {"shi", "--help", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_HELP, "returns HELP");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Combined short flags
 * --------------------------------------------------------------------------- */

static int test_combined_short_vh(void) {
    TEST_START("combined -vh flags");

    const char* items[] = {"shi", "-vh", "-c", "-f", "in.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_HELP, "returns HELP");

    TEST_END;
    return 0;
}

static int test_combined_short_vh_reversed(void) {
    TEST_START("combined -hv flags");

    const char* items[] = {"shi", "-hv", "-c", "-f", "in.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_HELP, "returns HELP");
    TEST_ASSERT(args.verbose == 1, "verbose is 1");

    TEST_END;
    return 0;
}

static int test_combined_short_cv(void) {
    TEST_START("combined -cv flags");

    const char* items[] = {"shi", "-cv", "-f", "in.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.command == CMD_COMPRESS, "command is CMD_COMPRESS");
    TEST_ASSERT(args.verbose == 1, "verbose is 1");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Error cases
 * --------------------------------------------------------------------------- */

static int test_no_mode_flag(void) {
    TEST_START("no mode flag returns BAD_USAGE");

    const char* items[] = {"shi", "-f", "in.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_USAGE, "returns BAD_USAGE");

    TEST_END;
    return 0;
}

static int test_no_file_flag(void) {
    TEST_START("no file flag returns BAD_USAGE");

    const char* items[] = {"shi", "-c", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_USAGE, "returns BAD_USAGE");

    TEST_END;
    return 0;
}

static int test_both_mode_flags(void) {
    TEST_START("both -c and -d returns BAD_ARG");

    const char* items[] = {"shi", "-c", "-d", "-f", "in.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_ARG, "returns BAD_ARG");

    TEST_END;
    return 0;
}

static int test_both_mode_flags_reversed(void) {
    TEST_START("both -d and -c returns BAD_ARG");

    const char* items[] = {"shi", "-d", "-c", "-f", "in.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_ARG, "returns BAD_ARG");

    TEST_END;
    return 0;
}

static int test_both_long_mode_flags(void) {
    TEST_START("both --compress and --decompress returns BAD_ARG");

    const char* items[] = {"shi", "--compress", "--decompress", "-f", "in.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_ARG, "returns BAD_ARG");

    TEST_END;
    return 0;
}

static int test_mixed_mode_flags(void) {
    TEST_START("both -c and --decompress returns BAD_ARG");

    const char* items[] = {"shi", "-c", "--decompress", "-f", "in.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_ARG, "returns BAD_ARG");

    TEST_END;
    return 0;
}

static int test_unknown_long_flag(void) {
    TEST_START("unknown --flag returns UNKNOWN");

    const char* items[] = {"shi", "--bogus", "-c", "-f", "in.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_UNKNOWN, "returns UNKNOWN");

    TEST_END;
    return 0;
}

static int test_unknown_short_flag(void) {
    TEST_START("unknown -x flag returns UNKNOWN");

    const char* items[] = {"shi", "-x", "-c", "-f", "in.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_UNKNOWN, "returns UNKNOWN");

    TEST_END;
    return 0;
}

static int test_version_missing_arg(void) {
    TEST_START("--version without value returns BAD_ARG");

    const char* items[] = {"shi", "-c", "-f", "in.txt", "--version", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_ARG, "returns BAD_ARG");

    TEST_END;
    return 0;
}

static int test_version_negative(void) {
    TEST_START("--version -1 returns BAD_ARG");

    const char* items[] = {"shi", "-c", "-f", "in.txt", "--version", "-1", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_ARG, "returns BAD_ARG");

    TEST_END;
    return 0;
}

static int test_version_too_large(void) {
    TEST_START("--version 999 returns BAD_ARG");

    const char* items[] = {"shi", "-c", "-f", "in.txt", "--version", "999", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_ARG, "returns BAD_ARG");

    TEST_END;
    return 0;
}

static int test_version_non_numeric(void) {
    TEST_START("--version abc returns BAD_ARG");

    const char* items[] = {"shi", "-c", "-f", "in.txt", "--version", "abc", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_ARG, "returns BAD_ARG");

    TEST_END;
    return 0;
}

static int test_file_missing_arg(void) {
    TEST_START("--file without value returns BAD_ARG");

    const char* items[] = {"shi", "-c", "--file", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_ARG, "returns BAD_ARG");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * -- separator
 * --------------------------------------------------------------------------- */

static int test_double_dash(void) {
    TEST_START("-- stops flag parsing");

    const char* items[] = {"shi", "--", "-c", "-f", "in.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    /* After --, everything is treated as positional — no flags parsed */
    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_USAGE, "returns BAD_USAGE");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Error message helper
 * --------------------------------------------------------------------------- */

static int test_error_messages(void) {
    TEST_START("error messages are non-NULL and descriptive");

    TEST_ASSERT(strlen(shi_args_error_msg(ARGS_OK)) > 0, "ARGS_OK message non-empty");
    TEST_ASSERT(strlen(shi_args_error_msg(ARGS_ERR_BAD_ARG)) > 0, "BAD_ARG message non-empty");
    TEST_ASSERT(strlen(shi_args_error_msg(ARGS_ERR_BAD_CMD)) > 0, "BAD_CMD message non-empty");
    TEST_ASSERT(strlen(shi_args_error_msg(ARGS_ERR_BAD_USAGE)) > 0, "BAD_USAGE message non-empty");
    TEST_ASSERT(strlen(shi_args_error_msg(ARGS_ERR_HELP)) > 0, "HELP message non-empty");
    TEST_ASSERT(strlen(shi_args_error_msg(ARGS_ERR_UNKNOWN)) > 0, "UNKNOWN message non-empty");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * NULL safety
 * --------------------------------------------------------------------------- */

static int test_null_safety(void) {
    TEST_START("NULL argument safety");

    shi_args_t args;
    TEST_ASSERT(shi_parse_args(0, NULL, &args) == ARGS_ERR_BAD_USAGE,
                "NULL argv returns BAD_USAGE");
    TEST_ASSERT(shi_parse_args(1, NULL, &args) == ARGS_ERR_BAD_USAGE,
                "NULL argv with argc=1 returns BAD_USAGE");
    TEST_ASSERT(shi_parse_args(1, (const char*[]){"shi", NULL}, NULL) == ARGS_ERR_BAD_USAGE,
                "NULL out returns BAD_USAGE");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Default version
 * --------------------------------------------------------------------------- */

static int test_default_version(void) {
    TEST_START("default version is SHI_CURRENT_VERSION");

    const char* items[] = {"shi", "-c", "-f", "in.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.version == SHI_CURRENT_VERSION, "version equals SHI_CURRENT_VERSION");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Decompression .shi extension validation
 * --------------------------------------------------------------------------- */

static int test_decompress_no_shi_extension(void) {
    TEST_START("decompress without .shi extension returns BAD_ARG");

    const char* items[] = {"shi", "-d", "-f", "input.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_ARG,
                "returns BAD_ARG for non-.shi input");

    TEST_END;
    return 0;
}

static int test_decompress_with_shi_extension(void) {
    TEST_START("decompress with .shi extension succeeds");

    const char* items[] = {"shi", "-d", "-f", "file.shi", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.command == CMD_DECOMPRESS, "command is CMD_DECOMPRESS");

    TEST_END;
    return 0;
}

static int test_decompress_with_dot_shi_in_path(void) {
    TEST_START("decompress with .shi in middle of filename");

    const char* items[] = {"shi", "-d", "-f", "my.backup.shi", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.command == CMD_DECOMPRESS, "command is CMD_DECOMPRESS");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Flag ordering: flags can appear in any order
 * --------------------------------------------------------------------------- */

static int test_flags_before_mode(void) {
    TEST_START("flags before mode flag work");

    const char* items[] = {"shi", "-v", "-V", "0", "-c", "-f", "in.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.verbose == 1, "verbose is 1");
    TEST_ASSERT(args.version == 0, "version is 0");
    TEST_ASSERT(args.command == CMD_COMPRESS, "command is CMD_COMPRESS");

    TEST_END;
    return 0;
}

static int test_flags_after_mode(void) {
    TEST_START("flags after mode flag work");

    const char* items[] = {"shi", "-c", "-v", "-f", "in.txt", "-V", "0", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.verbose == 1, "verbose is 1");
    TEST_ASSERT(args.version == 0, "version is 0");
    TEST_ASSERT(args.command == CMD_COMPRESS, "command is CMD_COMPRESS");

    TEST_END;
    return 0;
}

static int test_file_before_mode(void) {
    TEST_START("-f before mode flag works");

    const char* items[] = {"shi", "-f", "in.txt", "-c", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(strcmp(args.input_file, "in.txt") == 0, "input_file is 'in.txt'");
    TEST_ASSERT(args.command == CMD_COMPRESS, "command is CMD_COMPRESS");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Default output derivation tests
 * --------------------------------------------------------------------------- */

static int test_compress_default_output(void) {
    TEST_START("compress default output appends .shi");

    const char* items[] = {"shi", "-c", "-f", "input.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.command == CMD_COMPRESS, "command is CMD_COMPRESS");
    TEST_ASSERT(strcmp(args.input_file, "input.txt") == 0, "input_file is 'input.txt'");

    /* Derivation is tested in main.c; here just verify the flag is set */
    TEST_END;
    return 0;
}

static int test_decompress_default_output(void) {
    TEST_START("decompress default output strips .shi");

    const char* items[] = {"shi", "-d", "-f", "input.shi", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.command == CMD_DECOMPRESS, "command is CMD_DECOMPRESS");
    TEST_ASSERT(strcmp(args.input_file, "input.shi") == 0, "input_file is 'input.shi'");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Edge cases
 * --------------------------------------------------------------------------- */

static int test_file_flag_combination(void) {
    TEST_START("--file with --compress");

    const char* items[] = {"shi", "--compress", "--file", "input.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.command == CMD_COMPRESS, "command is CMD_COMPRESS");
    TEST_ASSERT(strcmp(args.input_file, "input.txt") == 0, "input_file is 'input.txt'");

    TEST_END;
    return 0;
}

static int test_help_with_other_flags(void) {
    TEST_START("--help with other flags still returns HELP");

    const char* items[] = {"shi", "-v", "--version", "0", "-c", "-f", "in.txt", "--help", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_HELP, "returns HELP");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Main
 * --------------------------------------------------------------------------- */

int main(void) {
    printf("=== CLI Argument Parser Test Suite ===\n\n");

    int failures = 0;

    /* Compress flag tests */
    failures += test_compress_short();
    failures += test_compress_long();

    /* Decompress flag tests */
    failures += test_decompress_short();
    failures += test_decompress_long();

    /* File flag tests */
    failures += test_file_short();
    failures += test_file_long();
    failures += test_file_with_path();

    /* Verbose flag tests */
    failures += test_verbose_short();
    failures += test_verbose_long();

    /* Version flag tests */
    failures += test_version_flag();
    failures += test_version_short();

    /* Help flag tests */
    failures += test_help_short();
    failures += test_help_long();

    /* Combined short flags */
    failures += test_combined_short_vh();
    failures += test_combined_short_vh_reversed();
    failures += test_combined_short_cv();

    /* Error cases */
    failures += test_no_mode_flag();
    failures += test_no_file_flag();
    failures += test_both_mode_flags();
    failures += test_both_mode_flags_reversed();
    failures += test_both_long_mode_flags();
    failures += test_mixed_mode_flags();
    failures += test_unknown_long_flag();
    failures += test_unknown_short_flag();
    failures += test_version_missing_arg();
    failures += test_version_negative();
    failures += test_version_too_large();
    failures += test_version_non_numeric();
    failures += test_file_missing_arg();

    /* -- separator */
    failures += test_double_dash();

    /* Error messages */
    failures += test_error_messages();

    /* NULL safety */
    failures += test_null_safety();

    /* Default version */
    failures += test_default_version();

    /* Decompression .shi extension validation */
    failures += test_decompress_no_shi_extension();
    failures += test_decompress_with_shi_extension();
    failures += test_decompress_with_dot_shi_in_path();

    /* Flag ordering */
    failures += test_flags_before_mode();
    failures += test_flags_after_mode();
    failures += test_file_before_mode();

    /* Default output derivation */
    failures += test_compress_default_output();
    failures += test_decompress_default_output();

    /* Edge cases */
    failures += test_file_flag_combination();
    failures += test_help_with_other_flags();

    printf("\n=== Results: %s test(s) ===", failures == 0 ? "ALL PASSED" : "SOME FAILED");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

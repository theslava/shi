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
 * Valid flag tests
 * --------------------------------------------------------------------------- */

static int test_default_args(void) {
    TEST_START("default args (no flags)");

    const char* items[] = {"shi", "compress", "in.txt", "out.huf", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    shi_args_result_t result = shi_parse_args(argc, argv, &args);
    TEST_ASSERT(result == ARGS_OK, "parse succeeds with no flags");
    TEST_ASSERT(args.verbose == 0, "verbose is 0 by default");
    TEST_ASSERT(args.version == SHI_CURRENT_VERSION, "version defaults to SHI_CURRENT_VERSION");
    TEST_ASSERT(args.command == CMD_COMPRESS, "command is CMD_COMPRESS");
    TEST_ASSERT(strcmp(args.input_file, "in.txt") == 0, "input_file is 'in.txt'");
    TEST_ASSERT(strcmp(args.output_file, "out.huf") == 0, "output_file is 'out.huf'");

    TEST_END;
    return 0;
}

static int test_compress_command(void) {
    TEST_START("compress command");

    const char* items[] = {"shi", "compress", "a.txt", "b.huf", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.command == CMD_COMPRESS, "command is CMD_COMPRESS");

    TEST_END;
    return 0;
}

static int test_decompress_command(void) {
    TEST_START("decompress command");

    const char* items[] = {"shi", "decompress", "a.huf", "b.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.command == CMD_DECOMPRESS, "command is CMD_DECOMPRESS");

    TEST_END;
    return 0;
}

static int test_verbose_short(void) {
    TEST_START("-v flag");

    const char* items[] = {"shi", "-v", "compress", "in.txt", "out.huf", NULL};
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

    const char* items[] = {"shi", "--verbose", "compress", "in.txt", "out.huf", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.verbose == 1, "verbose is 1");

    TEST_END;
    return 0;
}

static int test_version_flag(void) {
    TEST_START("--version 0 flag");

    const char* items[] = {"shi", "--version", "0", "compress", "in.txt", "out.huf", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_OK, "parse succeeds");
    TEST_ASSERT(args.version == 0, "version is 0");

    TEST_END;
    return 0;
}

static int test_help_short(void) {
    TEST_START("-h flag returns HELP");

    const char* items[] = {"shi", "-h", "compress", "in.txt", "out.huf", NULL};
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

    const char* items[] = {"shi", "--help", "compress", "in.txt", "out.huf", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_HELP, "returns HELP");

    TEST_END;
    return 0;
}

static int test_help_any_position(void) {
    TEST_START("--help after positional args is not parsed (BAD_USAGE)");

    const char* items[] = {"shi", "compress", "--help", "in.txt", "out.huf", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    /* --help after "compress" is treated as a positional arg (4 remaining, not 3) */
    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_USAGE, "returns BAD_USAGE");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Combined short flags
 * --------------------------------------------------------------------------- */

static int test_combined_short_vh(void) {
    TEST_START("combined -vh flags");

    const char* items[] = {"shi", "-vh", "compress", "in.txt", "out.huf", NULL};
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

    const char* items[] = {"shi", "-hv", "compress", "in.txt", "out.huf", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_HELP, "returns HELP");
    TEST_ASSERT(args.verbose == 1, "verbose is 1");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Error cases
 * --------------------------------------------------------------------------- */

static int test_unknown_long_flag(void) {
    TEST_START("unknown --flag returns UNKNOWN");

    const char* items[] = {"shi", "--bogus", "compress", "in.txt", "out.huf", NULL};
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

    const char* items[] = {"shi", "-x", "compress", "in.txt", "out.huf", NULL};
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

    const char* items[] = {"shi", "--version", "compress", "in.txt", "out.huf", NULL};
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

    const char* items[] = {"shi", "--version", "-1", "compress", "in.txt", "out.huf", NULL};
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

    const char* items[] = {"shi", "--version", "999", "compress", "in.txt", "out.huf", NULL};
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

    const char* items[] = {"shi", "--version", "abc", "compress", "in.txt", "out.huf", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_ARG, "returns BAD_ARG");

    TEST_END;
    return 0;
}

static int test_bad_command(void) {
    TEST_START("unknown command returns BAD_CMD");

    const char* items[] = {"shi", "foo", "in.txt", "out.huf", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_CMD, "returns BAD_CMD");

    TEST_END;
    return 0;
}

static int test_missing_positional_args(void) {
    TEST_START("too few positional args returns BAD_USAGE");

    const char* items[] = {"shi", "compress", "in.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_USAGE, "returns BAD_USAGE");

    TEST_END;
    return 0;
}

static int test_too_many_positional_args(void) {
    TEST_START("too many positional args returns BAD_USAGE");

    const char* items[] = {"shi", "compress", "in.txt", "out.huf", "extra.txt", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_USAGE, "returns BAD_USAGE");

    TEST_END;
    return 0;
}

static int test_no_command(void) {
    TEST_START("no command returns BAD_USAGE");

    const char* items[] = {"shi", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    TEST_ASSERT(shi_parse_args(argc, argv, &args) == ARGS_ERR_BAD_USAGE, "returns BAD_USAGE");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * -- separator
 * --------------------------------------------------------------------------- */

static int test_double_dash(void) {
    TEST_START("-- stops flag parsing");

    const char* items[] = {"shi", "--", "compress", "in.txt", "out.huf", NULL};
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
 * Flag ordering: flags after positional args should not be parsed
 * --------------------------------------------------------------------------- */

static int test_flag_after_positional(void) {
    TEST_START("flag after positional args is treated as arg");

    const char* items[] = {"shi", "compress", "-v", "in.txt", "out.huf", NULL};
    int argc;
    const char** argv;
    make_argv(items, &argc, &argv);
    shi_args_t args;

    /* -v after "compress" is treated as a positional arg, so we get 4 args instead of 3 */
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

    const char* items[] = {"shi", "compress", "in.txt", "out.huf", NULL};
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
 * Main
 * --------------------------------------------------------------------------- */

int main(void) {
    printf("=== CLI Argument Parser Test Suite ===\n\n");

    int failures = 0;

    /* Valid flags */
    failures += test_default_args();
    failures += test_compress_command();
    failures += test_decompress_command();
    failures += test_verbose_short();
    failures += test_verbose_long();
    failures += test_version_flag();
    failures += test_help_short();
    failures += test_help_long();
    failures += test_help_any_position();

    /* Combined short flags */
    failures += test_combined_short_vh();
    failures += test_combined_short_vh_reversed();

    /* Error cases */
    failures += test_unknown_long_flag();
    failures += test_unknown_short_flag();
    failures += test_version_missing_arg();
    failures += test_version_negative();
    failures += test_version_too_large();
    failures += test_version_non_numeric();
    failures += test_bad_command();
    failures += test_missing_positional_args();
    failures += test_too_many_positional_args();
    failures += test_no_command();

    /* -- separator */
    failures += test_double_dash();

    /* Flag ordering */
    failures += test_flag_after_positional();

    /* Error messages */
    failures += test_error_messages();

    /* NULL safety */
    failures += test_null_safety();

    /* Default version */
    failures += test_default_version();

    printf("\n=== Results: %s test(s) ===", failures == 0 ? "ALL PASSED" : "SOME FAILED");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

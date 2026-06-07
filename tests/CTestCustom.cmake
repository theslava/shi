# CTest Custom configuration for shi
# This file is for local customization and is not checked into version control.

# Ignore these test files (not yet ready)
set(CTEST_CUSTOM_IGNORED_TESTS
    "test_utils"
)

# Custom test output directory
set(CTEST_CUSTOM_COVERAGE_CTEST_OUTPUT_DIRECTORY
    "${CTEST_BINARY_DIR}/coverage"
)

# Custom memory check tool (optional)
# set(CTEST_MEMORYCHECK_COMMAND valgrind)
# set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --error-exitcode=1")

# Suppress warnings about unused variables in tests
set(CTEST_CUSTOM_WARNING_EXCEPTION
    ".*unused variable.*"
    ".*unused parameter.*"
)

# Maximum number of test failures before stopping
set(CTEST_CUSTOM_MAXIMUM_PASSED_TESTS 0)
set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS 0)
set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_ERRORS 0)

# Custom test failure output
set(CTEST_CUSTOM_TEST_FAILURE_REGEXP
    "^\[FAILED\].*"
)

# Include test dependencies
include("${CTEST_SOURCE_DIR}/tests/CTestConfig.cmake")
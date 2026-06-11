# CTest Configuration file for shi (Slava's Huffman Implementation)
# This file provides metadata about the project for CTest.

set(CTEST_PROJECT_NAME "shi")
set(CTEST_NIGHTLY_START_TIME "01:00:00 UTC")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "my.cdash.org")
set(CTEST_DROP_LOCATION "/submit.php?project=shi")
set(CTEST_DROP_SITE_CDASH TRUE)

# Project owner information
set(CTEST_PROJECT_SUBPROJECTS
    "core"
    "data_structures"
    "io"
    "utils"
)

# Test timeout (in seconds)
set(CTEST_TEST_TIMEOUT 30)
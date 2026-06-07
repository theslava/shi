#!/bin/bash
# Test runner script for shi (Slava's Huffman Implementation)
# This script compiles and runs all test suites.

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Project directories
SRC_DIR="../src"
INCLUDE_DIR="../include"
BUILD_DIR="build"

# Test source files
TEST_SOURCES=(
    "test_bitstream.c"
    "test_compress.c"
    "test_file_reader.c"
    "test_file_writer.c"
    "test_list.c"
    "test_tree.c"
    "test_utils.c"
)

# Common compiler flags
CFLAGS="-Wall -Wextra -std=c99 -I${INCLUDE_DIR} -g"

echo "========================================"
echo "  shi Test Runner"
echo "========================================"
echo ""

# Create build directory
mkdir -p "${BUILD_DIR}"

TOTAL_TESTS=${#TEST_SOURCES[@]}
PASSED=0
FAILED=0
SKIPPED=0

for test_src in "${TEST_SOURCES[@]}"; do
    test_name="${test_src%.c}"
    test_exec="${BUILD_DIR}/${test_name}"
    
    echo -e "${YELLOW}Compiling ${test_src}...${NC}"
    
    # Determine which source files to include based on the test
    case "${test_name}" in
        test_bitstream)
            test_deps="../src/io/file_io.c ../src/data_structures/bitstream.c"
            ;;
        test_compress)
            test_deps="../src/core/compress.c ../src/core/decompress.c"
            ;;
        test_file_reader|test_file_writer)
            test_deps="../src/io/file_io.c"
            ;;
        test_list)
            test_deps="../src/data_structures/list.c ../src/data_structures/node.c"
            ;;
        test_tree)
            test_deps="../src/data_structures/tree.c ../src/data_structures/node.c"
            ;;
        test_utils)
            test_deps="../src/utils/metric.c ../src/utils/sort.c"
            ;;
        *)
            test_deps=""
            ;;
    esac
    
    # Compile the test
    if gcc ${CFLAGS} "${test_src}" ${test_deps} -o "${test_exec}" 2>/dev/null; then
        echo -e "${GREEN}✓ Compiled successfully${NC}"
    else
        echo -e "${RED}✗ Compilation failed${NC}"
        FAILED=$((FAILED + 1))
        continue
    fi
    
    # Run the test
    echo -e "${YELLOW}Running ${test_name}...${NC}"
    if "${test_exec}"; then
        echo -e "${GREEN}✓ ${test_name} PASSED${NC}"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}✗ ${test_name} FAILED${NC}"
        FAILED=$((FAILED + 1))
    fi
    
    # Clean up executable
    rm -f "${test_exec}"
    echo ""
done

# Summary
echo "========================================"
echo "  Test Summary"
echo "========================================"
echo -e "Total:  ${TOTAL_TESTS}"
echo -e "${GREEN}Passed: ${PASSED}${NC}"
echo -e "${RED}Failed: ${FAILED}${NC}"
echo -e "${YELLOW}Skipped: ${SKIPPED}${NC}"
echo "========================================"

if [ ${FAILED} -gt 0 ]; then
    echo -e "${RED}Some tests failed!${NC}"
    exit 1
else
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
fi"
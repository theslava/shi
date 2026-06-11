#!/bin/bash
# Clean build artifacts for shi

set -e

echo "Cleaning build artifacts..."

# Remove build directory
rm -rf build/

# Remove any stray object files
find . -name '*.o' -delete

# Remove executables
rm -f compressor
rm -f test_bitstream

echo "Clean complete."
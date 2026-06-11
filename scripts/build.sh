#!/bin/bash
# Build script for shi (Slava's Huffman Implementation)

set -e

echo "Building shi..."

# Create build directories
mkdir -p build/bin
mkdir -p build/obj
mkdir -p build/lib

# Run make
make all

echo "Build complete. Executable: build/bin/compressor"
echo "Run: build/bin/compressor compress <input> <output>"
echo "Run: build/bin/compressor decompress <input> <output>"
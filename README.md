# shi — Huffman Compression Implementation

A complete, production-ready **Huffman compression/decompression** tool written in C.

## Overview

**shi** (Slava's Huffman Implementation) implements a full Huffman coding pipeline in C — from frequency analysis and tree construction to bitstream encoding/decoding. It reads any binary file, builds an optimal prefix code, and produces a compressed output that can be perfectly reconstructed.

This project was created to:

- Demonstrate core data structures (trees, heaps, linked lists) in C
- Provide a working reference implementation of Huffman coding
- Exercise low-level I/O, bit manipulation, and memory management patterns
- Serve as a learning tool for understanding lossless compression algorithms

## Architecture

The codebase is organized into modular layers:

| Layer | Modules | Responsibility |
| ------- | --------- | --------------- |
| **I/O** | `file_io.c/h` | Buffered file reader (`fr_fd`) and writer (`fw_fd`) |
| **Data Structures** | `node.c/h`, `tree.c/h`, `list.c/h` | Huffman tree node, tree builder, sorted doubly-linked list |
| **Sorting** | `sort.c/h` | Heapsort helpers for ordering nodes by frequency weight |
| **Metrics** | `metric.c/h` | Byte-frequency counter over input data |
| **Bitstream** | `bitstream.c/h`, `bitarray.c/h` | MSB-first bit-level reader/writer with auto-flush |
| **Core** | `compress.c/h`, `decompress.c/h` | High-level compression & decompression pipelines |

### Compression Pipeline

```text
Input file → frequency counting → build Huffman tree → generate codes
           → write header → bitstream encode → Output file
```

### Decompression Pipeline

```text
Compressed input → read header → reconstruct tree → bitstream decode → Output file
```

### Header Format

The compressed file header stores:

- Magic bytes: `"SHI\x00"` (4B, `0x53, 0x48, 0x49, 0x00`)
- `num_symbols` (4B LE) — number of unique bytes in the original
- `file_size` (4B LE) — original file size (to stop decoding at the right point)
- Per symbol: `byte_value` (1B) + `code_length` (1B) + `code_value` (4B LE)

## Tools Used

- **Language**: C99
- **Build System**: CMake (primary) with cross-platform wrappers
- **Compiler Flags**: `-Wall -Wextra -g` (GCC/Clang), `/W4` (MSVC)
- **Testing**: Custom test suite (7 test executables, 20+ test cases)
- **Test Runners**: `build.ps1` (Windows), `make test` (Unix/macOS)

## Build Instructions

### Prerequisites

- CMake ≥ 3.15
- A C compiler (GCC, Clang, or MSVC)
- Ninja (optional, auto-detected for faster builds)

### Build

```powershell
# Windows (PowerShell)
.\build.ps1

# Or using CMake directly
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Unix/macOS (Makefile)
make
```

The main executable `shi` will be built in the `build` directory (or current directory depending on your CMake generator).

### Build Options

```powershell
# Verbose build (show full compiler commands)
.\build.ps1 build -ShowVerbose

# Release build
.\build.ps1 build -BuildType Release

# Show help
.\build.ps1 help
```

```bash
# Verbose build (show full compiler commands)
make build VERBOSE=1

# Release build
make build BUILD_TYPE=Release
```

### Running Tests

```powershell
# Windows
.\build.ps1 test

# Run a specific test
.\build.ps1 test bitstream    # test_bitstream
.\build.ps1 test compress     # test_compress
```

```bash
# Unix/macOS
make test

# Run a specific test
make test-bitstream
make test-compress
```

## Usage

### Compress a file

```bash
./shi compress input.txt output.huf
```

### Decompress a file

```bash
./shi decompress output.huf recovered.txt
```

## Project Status

**Core Implementation: Complete** ✅

- ✅ Full compression pipeline (`compress_file()`)
- ✅ Full decompression pipeline (`decompress_file()`)
- ✅ Error handling with NULL checks and status codes throughout
- ✅ All 7 test suites passing (100%)
- ✅ Cross-platform build system (CMake + PowerShell + Makefile)
- ✅ Magic byte validation on decompression

### Known Limitations

- Single-symbol edge case is handled but could be more robust
- No `--verbose` / progress output option yet
- Entire file is loaded into memory for frequency analysis (fine for the 256-byte alphabet)

### Future Enhancements (v2+)

- Command-line options: custom buffer sizes, `--verbose`, stdin/stdout support
- Unit tests for `generate_codes()` and `reconstruct_tree_from_codes()`
- Integration tests with known-good compressed output
- Performance optimization: switch from insertion sort to heapsort in tree building

## File Index

```
include/
├── core/
│   ├── compress.h      — compress_file, write_header, compress_data, read_header, etc.
│   └── decompress.h    — decompress_file, reconstruct_tree_from_codes
├── data_structures/
│   ├── bitarray.h      — bit array operations
│   ├── bitstream.h     — bit-level reader + writer
│   ├── list.h          — doubly-linked sorted list
│   ├── node.h          — Huffman tree node
│   └── tree.h          — tree builder + code generator
├── io/
│   └── file_io.h       — buffered reader + writer
└── utils/
    ├── metric.h        — frequency counter
    └── sort.h          — heapsort helpers

src/
├── core/
│   ├── compress.c
│   └── decompress.c
├── data_structures/
│   ├── bitarray.c
│   ├── bitstream.c
│   ├── list.c
│   ├── node.c
│   └── tree.c
├── io/
│   └── file_io.c
├── utils/
│   ├── metric.c
│   └── sort.c
└── main.c              — CLI entry point

tests/
├── test_compress.c     — 3 tests (roundtrip, empty, repeated)
├── test_bitstream.c    — 7 tests (reader/writer, EOF, NULL)
├── test_file_reader.c  — 5 tests
├── test_file_writer.c  — 4 tests
├── test_list.c         — 5 tests
├── test_tree.c         — 3 tests
├── test_utils.c        — 2 tests
└── test_helpers.h      — temp files, comparison, macros

docs/
├── Architecture.md     — module responsibilities, data flow, design decisions
├── Changelog.md        — history of bug fixes and changes
└── Roadmap.md          — phased work items and future plans

build.ps1               — PowerShell build/test/clean script
Makefile                — CMake wrapper (Unix/macOS)
CMakeLists.txt          — primary build system
scripts/clean.ps1       — PowerShell cleanup utility
```

## License

This project is provided as-is for educational and reference purposes.

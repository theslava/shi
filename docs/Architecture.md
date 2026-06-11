# Architecture

## Module Responsibilities

| Module | Responsibility |
|--------|---------------|
| `file_io.c / file_io.h` | Low-level buffered I/O (read + write) |
| `metric.c` | Count byte frequencies from input data |
| `node.c` | Huffman tree node struct & helpers |
| `sort.c` | Heap-based sorting of nodes by weight |
| `list.c` | Doubly-linked sorted list for building the tree |
| `tree.c` | Build Huffman tree from frequency metric; generate codes |
| `bitstream.c` | Bit-level reader + writer (MSB-first) |
| `bitarray.c` | Bit array operations + `ba_write_to_file()` |
| `compress.c` | High-level compression pipeline |
| `decompress.c` | High-level decompression pipeline |

## Data Flow

### Compression
```
Input file → fr_new() → metric (frequency counts)
           → sort nodes → build Huffman tree → generate_codes()
           → write_header() + compress_data() via bitstream writer → Output file
```

### Decompression
```
Compressed input → read_header() → reconstruct_tree_from_codes()
                 → decompress_data() via bitstream reader → fw_write_byte() → Output file
```

## Key Design Decisions

1. **MSB-first bit ordering**: Bits are written/read starting from the most significant bit of each byte.
2. **Buffered I/O**: Both reader and writer use a configurable buffer size (default 4096) to minimize system calls.
3. **Static node array in tree**: `tree->nodes[512]` is allocated as part of the `tree` struct.
4. **Header format**: `[4B LE: num_symbols] [4B LE: file_size] [per symbol: 1B byte_value + 1B code_length + 4B LE code_value]`.
5. **Error handling**: Every function returns a status indicator (`NULL` on allocation failure, `-1` on I/O error).
6. **Cross-platform compatibility**: Build system uses CMake with MinGW Makefiles on all platforms.

## File Index

```
include/
├── core/
│   ├── compress.h      ✓ complete
│   └── decompress.h    ✓ complete
├── data_structures/
│   ├── bitarray.h      ✓ complete
│   ├── bitstream.h     ✓ complete
│   ├── list.h          ✓ complete
│   ├── node.h          ✓ complete
│   └── tree.h          ✓ complete
├── io/
│   └── file_io.h       ✓ complete (reader + writer)
├── utils/
│   ├── metric.h        ✓ complete
│   └── sort.h          ✓ complete

src/
├── core/
│   ├── compress.c      ✓ complete
│   └── decompress.c    ✓ complete
├── data_structures/
│   ├── bitarray.c      ⚠️ partial
│   ├── bitstream.c     ✓ complete
│   ├── list.c          ✓ complete
│   ├── node.c          ✓ complete
│   └── tree.c          ✓ complete
├── io/
│   └── file_io.c       ✓ complete
├── main.c              ✓ complete
└── utils/
    ├── metric.c        ✓ complete
    └── sort.c          ✓ complete

tests/
├── test_compress.c     ✓ complete
├── test_helpers.h      ✓ complete
├── test_bitstream.c    ✓ complete
├── test_file_reader.c  ✓ complete
├── test_file_writer.c  ✓ complete
├── test_list.c         ✓ complete
├── test_tree.c         ✓ complete
└── test_utils.c        ✓ complete

docs/
└── Plan.md             — legacy plan

Makefile                ✓ CMake wrapper
CMakeLists.txt          ✓ primary build system
tests/run_tests.sh      ✓ Bash test runner
tests/run_tests.ps1     ✓ PowerShell test runner
```

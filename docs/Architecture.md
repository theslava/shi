# Architecture

## Module Responsibilities

| Module | Responsibility |
|--------|---------------|
| `file_io.c / file_io.h` | Buffered file reader (`fr_fd`) and writer (`fw_fd`) with configurable buffer sizes |
| `metric.c / metric.h` | Count byte frequencies from input data |
| `node.c / node.h` | Huffman tree node struct (`byte`, `weight`, `left`, `right`, `next`) and helpers |
| `sort.c / sort.h` | Heapsort helpers for ordering nodes by weight; `sort_nodes_by_weight()` wrapper |
| `list.c / list.h` | Doubly-linked sorted list for building the tree |
| `tree.c / tree.h` | Build Huffman tree from frequency metric; `generate_codes()` traversal |
| `bitstream.c / bitstream.h` | Bit-level reader (`bitstream`) + writer (`bitstream_writer`), MSB-first |
| `bitarray.c / bitarray.h` | Bit array operations + `ba_write_to_file()` (bit-to-byte packing) |
| `compress.c / compress.h` | High-level compression pipeline: `compress_file()`, `write_header()`, `compress_data()` |
| `decompress.c / decompress.h` | High-level decompression pipeline: `decompress_file()`, `read_header()`, `reconstruct_tree_from_codes()`, `decompress_data()` |

## Data Flow

### Compression

```
Input file → fr_new() → metric (frequency counts)
           → sort nodes → build Huffman tree → generate_codes()
           → write_header() [magic + num_symbols + file_size + per-symbol codes]
           → compress_data() via bitstream writer → Output file
```

### Decompression

```
Compressed input → read_header() [magic + num_symbols + file_size + per-symbol codes]
                 → reconstruct_tree_from_codes()
                 → decompress_data() via bitstream reader → fw_write_byte() → Output file
```

## Key Design Decisions

1. **MSB-first bit ordering**: Bits are written/read starting from the most significant bit of each byte.
2. **Buffered I/O**: Both reader and writer use a configurable buffer size (default 4096) to minimize system calls.
3. **Static node array in tree**: `tree->nodes[512]` is allocated as part of the `tree` struct.
4. **Header format**: `[4B LE: num_symbols] [4B LE: file_size] [per symbol: 1B byte_value + 1B code_length + 4B LE code_value]`.
5. **Magic bytes**: All `.huf` files start with `"SHI\x00"` (0x53, 0x48, 0x49, 0x00) for format identification.
6. **Error handling**: Every function returns a status indicator (`NULL` on allocation failure, `-1` on I/O error).
7. **Cross-platform compatibility**: Build system is CMake-only. CMake auto-detects the platform and selects the appropriate generator (Ninja, MSVC, or GCC/Clang) and compiler flags.

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
└── utils/
    ├── metric.h        ✓ complete
    └── sort.h          ✓ complete

src/
├── core/
│   ├── compress.c      ✓ complete
│   └── decompress.c    ✓ complete
├── data_structures/
│   ├── bitarray.c      ✓ complete
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
├── test_compress.c     ✓ complete (3 tests: roundtrip, empty, repeated)
├── test_helpers.h      ✓ complete
├── test_bitstream.c    ✓ complete (7 tests: reader/writer, EOF, NULL)
├── test_file_reader.c  ✓ complete (5 tests)
├── test_file_writer.c  ✓ complete (4 tests)
├── test_list.c         ✓ complete (5 tests)
├── test_tree.c         ✓ complete (3 tests)
└── test_utils.c        ✓ complete (2 tests)

docs/
├── Architecture.md     — module responsibilities, data flow, design decisions
├── Changelog.md        — history of bug fixes and changes
└── Roadmap.md          — phased work items and future plans

CMakeLists.txt          — CMake build configuration (sources, tests, compiler flags, install)
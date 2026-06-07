# Plan.md — Huffman Compression Project

## 1. Plan

The goal is to implement a complete **Huffman compression/decompression** tool in C with the following architecture:

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
| `dictionary.h` | Placeholder for future dictionary-based compression |

**Data flow — Compression:**
```
Input file → fr_new() → metric (frequency counts)
           → sort nodes → build Huffman tree → generate_codes()
           → write_header() + compress_data() via bitstream writer → Output file
```

**Data flow — Decompression:**
```
Compressed input → read_header() → reconstruct_tree_from_codes()
                 → decompress_data() via bitstream reader → fw_write_byte() → Output file
```

---

## 2. What Was Done

### Headers (`include/`)

| File | Status |
|------|--------|
| `file_io.h` | ✓ **Complete** — reader (`fr_fd`) and writer (`fw_fd`) structs + full API |
| `node.h` | ✓ **Complete** — node struct, `new_node()`, `delete_node()`, `get_weight()`, `compare_nodes()` declared |
| `sort.h` | ✓ **Complete** — heapsort helpers + `sort_nodes_by_weight()` declared |
| `list.h` | ✓ **Complete** — doubly-linked list with `count`, insertion, removal, append |
| `tree.h` | ✓ **Complete** — tree struct with static node array, `new_tree()`, `generate_codes()`, `free_tree_nodes()` declared |
| `bitstream.h` | ✓ **Complete** — full reader (`bitstream`) + writer (`bitstream_writer`) API (MSB-first bit packing) |
| `bitarray.h` | ✓ **Complete** — bit array operations + `ba_write_to_file()` declared (stub implementation) |
| `metric.h` | ✓ **Complete** — frequency counter struct + helpers |
| `compress.h` | ✓ **Complete** — `compress_file()`, `write_header()`, `compress_data()`, `read_header()`, `decompress_data()`, `reconstruct_tree_from_codes()` |
| `decompress.h` | ✓ **Complete** — `decompress_file()`, `reconstruct_tree_from_codes()` |
| `dictionary.h` | ⚠️ **Empty** — placeholder file, no implementation yet |

### Implementation Files (`src/`)

| File | Status |
|------|--------|
| `file_io.c` | ✓ **Complete** — buffered reader/writer with `fr_new()`, `fr_read()`, `fr_rewind()`, `fr_done()`, `fr_info()`, `fw_new()`, `fw_write_byte()`, `fw_write_bytes()`, `fw_flush()`, `fw_done()` |
| `bitstream.c` | ✓ **Complete** — full reader + writer (MSB-first bit packing, auto-flush, EOF handling) |
| `metric.c` | ✓ **Complete** — frequency counting from file (`new_metric()`, `new_metric_from_file()`, `fill_metric()`, `delete_metric()`) |
| `node.c` | ✓ **Complete** — `new_node()` allocates & initializes, `delete_node()` frees, `get_weight()`, `compare_nodes()` |
| `sort.c` | ✓ **Complete** — `sort_nodes_by_weight()` uses insertion sort, `heapsort()` + `heapify()` + `swap()` helpers |
| `list.c` | ✓ **Complete** — doubly-linked list with `new_list()`, `delete_list()`, `new_list_from_array()`, `insert_node()`, `remove_node()`, `list_append()`, `list_size()`, `list_get_head()` |
| `tree.c` | ✓ **Complete** — `new_tree()`, `delete_tree()`, `tree_insert()`, `new_tree_from_metric()` (builds Huffman tree from frequency metric), `generate_codes()` (DFS traversal, assigns 0/1 codes), `free_tree_nodes()` |
| `compress.c` | ✓ **Complete** — `compress_file()` full call chain, `write_header()` (4-byte LE symbol count + symbol/code-length pairs), `compress_data()` (bitstream writer loop) |
| `decompress.c` | ✓ **Complete** — `decompress_file()` full call chain, `read_header()` (parses header format), `decompress_data()` (tree traversal bit-by-bit), `reconstruct_tree_from_codes()` (builds tree from codes) |
| `bitarray.c` | ⚠️ **Partial** — all bit operations work, but `ba_write_to_file()` is a stub (returns -1) |
| `main.c` | ✓ **Complete** — CLI entry point with `compress`/`decompress` commands |

### Build System

- **CMakeLists.txt**: Primary build system (CMake-based) with main executable + 7 test executables
- **Makefile**: Wrapper that invokes CMake (cross-platform, uses MinGW Makefiles on all platforms)

### Test Files

| File | Status |
|------|--------|
| `tests/test_compress.c` | ✓ **Complete** — 3 tests: roundtrip, empty file, repeated content |
| `tests/test_helpers.h` | ✓ **Complete** — temp file creation, file comparison, test macros |
| `tests/test_bitstream.c` | ✓ **Complete** — 7 tests: reader/writer, EOF handling, NULL pointer handling |
| `tests/test_file_reader.c` | ✓ **Complete** — 5 tests: create/destroy, read byte, read all, small buffer, NULL handling |
| `tests/test_file_writer.c` | ✓ **Complete** — 4 tests: create/destroy, write byte, write multiple, NULL handling |
| `tests/test_list.c` | ✓ **Complete** — 5 tests: create/destroy, append, pop, pop empty, NULL handling |
| `tests/test_tree.c` | ✓ **Complete** — 3 tests: create/destroy, insert, build from frequencies (NULL handling test included) |
| `tests/test_utils.c` | ✓ **Complete** — 2 tests: sort nodes, NULL handling (metric test commented out) |

### Examples

| File | Status |
|------|--------|
| `examples/basic_usage.c` | ✓ **Complete** — demonstrates compress → decompress cycle |
| `examples/advanced_usage.c` | ⚠️ **Partial** — has compilation issues (references `file_reader.h`/`file_writer.h` which don't exist; should use `file_io.h`) |

---

## 3. Current Status: **Core Implementation Complete**

All core Huffman compression/decompression functionality is implemented and functional:

- ✅ **Compression pipeline**: `compress_file()` → metric → tree → codes → header → bitstream write
- ✅ **Decompression pipeline**: `decompress_file()` → header read → tree reconstruction → bitstream read
- ✅ **Error handling**: All functions return status codes (`0` = success, `-1` = error), NULL checks throughout
- ✅ **Single-symbol edge case**: Handled in `generate_codes()` and `decompress_data()`
- ✅ **Test suite**: 7 test executables covering all modules
- ✅ **Build system**: CMake + Makefile wrapper

---

## 4. Remaining Work

### Phase 1 — Bug Fixes & Polish (highest priority)

| Issue | File | Description |
|-------|------|-------------|
| `advanced_usage.c` includes wrong headers | `examples/advanced_usage.c` | References `file_reader.h`/`file_writer.h` — should use `file_io.h` |
| `ba_write_to_file()` is a stub | `src/data_structures/bitarray.c` | Returns -1; needs implementation to write bitarray data to file |
| `delete_node()` has formatting issue | `src/data_structures/node.c` | Missing closing brace indentation (cosmetic, not functional) |
| `test_tree.c` has commented-out test | `tests/test_tree.c` | `test_tree_insert()` and `test_tree_build()` assertions are incomplete |
| `test_utils.c` has commented-out test | `tests/test_utils.c` | `test_metric()` is commented out; references non-existent `metric_calculate()` |
| `new_tree_from_metric()` doesn't handle all-zero frequencies | `src/data_structures/tree.c` | If all frequencies are zero, the tree may not be built correctly |

### Phase 2 — Edge Cases & Robustness

- Handle single-symbol input more robustly (tree with only one leaf node)
- Validate header integrity on decompression (check symbol count matches expected)
- Add progress/logging output options (e.g., `--verbose` flag)
- Handle very large files (current implementation loads entire file into frequency metric, which is fine for 256-byte alphabet but could be optimized)
- Address any compiler warnings from `-Wall -Wextra` (especially unused parameter warnings in `insert_node()`)

### Phase 3 — Testing Improvements

- Write unit tests for `generate_codes()` specifically
- Write unit tests for `reconstruct_tree_from_codes()` specifically
- Add tests for edge cases: empty file, single-byte file, all-same-byte file, binary data
- Add integration tests with known-good compressed output
- Fix commented-out tests in `test_tree.c` and `test_utils.c`

### Phase 4 — Future Enhancements (out of scope for v1)

- Implement `dictionary.h` for dictionary-based compression (e.g., LZ77 + Huffman)
- Add command-line options for custom buffer sizes, verbose output
- Support for reading from stdin / writing to stdout
- Add `ba_write_to_file()` implementation for bitarray persistence
- Consider switching from insertion sort to heapsort in `new_tree_from_metric()` for better performance on large alphabets

---

## 5. Key Design Decisions

1. **MSB-first bit ordering** — bits are written/read starting from the most significant bit of each byte (consistent with standard practices).
2. **Buffered I/O** — both reader and writer use a configurable buffer size (default 4096) to minimize system calls.
3. **Static node array in tree** — `tree->nodes[512]` is allocated as part of the `tree` struct. Only internal parent nodes need dynamic allocation, but the current implementation stores them in the static array which creates a `free_tree_nodes()` problem (nodes in the array don't need freeing).
4. **Header format** — 4-byte LE integer for symbol count, followed by pairs of (byte value: 1 byte, code length: 1 byte) for each distinct symbol. This is compact and easy to parse.
5. **Error handling** — every function returns a status indicator (`NULL` on allocation failure, `-1` on I/O error). The top-level `compress_file()` / `decompress_file()` clean up all resources before returning.
6. **Cross-platform compatibility** — build system uses CMake with MinGW Makefiles on all platforms; conditional compilation for Windows POSIX functions (`_CRT_SECURE_NO_WARNINGS`, `open()` mode flags).

---

## 6. File Index

```
include/
├── core/
│   ├── compress.h      ✓ complete (compress_file, write_header, compress_data, read_header, decompress_data, reconstruct_tree_from_codes)
│   └── decompress.h    ✓ complete (decompress_file, reconstruct_tree_from_codes)
├── data_structures/
│   ├── bitarray.h      ✓ complete (ba_write_to_file stub)
│   ├── bitstream.h     ✓ complete (reader + writer)
│   ├── list.h          ✓ complete
│   ├── node.h          ✓ complete
│   └── tree.h          ✓ complete
├── io/
│   └── file_io.h       ✓ complete (reader + writer)
├── utils/
│   ├── metric.h        ✓ complete
│   └── sort.h          ✓ complete
└── dictionary.h        ⚠️ empty (placeholder)

src/
├── core/
│   ├── compress.c      ✓ complete (compress_file, write_header, compress_data)
│   └── decompress.c    ✓ complete (decompress_file, read_header, decompress_data, reconstruct_tree_from_codes)
├── data_structures/
│   ├── bitarray.c      ⚠️ partial (ba_write_to_file stub)
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
├── test_compress.c     ✓ complete (3 tests)
├── test_helpers.h      ✓ complete
├── test_bitstream.c    ✓ complete (7 tests)
├── test_file_reader.c  ✓ complete (5 tests)
├── test_file_writer.c  ✓ complete (4 tests)
├── test_list.c         ✓ complete (5 tests)
├── test_tree.c         ✓ complete (3 tests, some commented out)
└── test_utils.c        ✓ complete (2 tests, 1 commented out)

examples/
├── basic_usage.c       ✓ complete
└── advanced_usage.c    ⚠️ partial (wrong header includes)

docs/
└── Plan.md             — this file

Makefile                ✓ CMake wrapper
CMakeLists.txt          ✓ primary build system (main executable + 7 test executables)
```
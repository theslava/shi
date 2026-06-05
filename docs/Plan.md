# Agent.md — Huffman Compression Project

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
| `compress.c` | High-level compression pipeline |
| `decompress.c` | High-level decompression pipeline |

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
| `file_io.h` | ✓ Complete — reader (`fr_fd`) and writer (`fr_wd`) structs + full API |
| `node.h` | ✓ Complete — node struct, `new_node()`, `delete_node()` declared |
| `sort.h` | ✓ Complete — heapsort helpers + `sort_nodes_by_weight()` declared |
| `list.h` | ✓ Complete — doubly-linked list with `count`, insertion, removal |
| `tree.h` | ✓ Complete — tree struct with static node array, `new_tree()`, `generate_codes()`, `free_tree_nodes()` |
| `bitstream.h` | ✓ Complete — full reader + writer API (MSB-first bit packing) |
| `bitarray.h` | ✓ Complete — bit array operations + `ba_write_to_file()` |
| `metric.h` | ✓ Complete — frequency counter struct + helpers |
| `compress.h` | ✓ Complete — `compress_file()` + internal helpers (`write_header()`, `compress_data()`) |
| `decompress.h` | ✓ Complete — `decompress_file()` + `reconstruct_tree_from_codes()` |

### Implementation Files (`src/`)

| File | Status |
|------|--------|
| `file_io.c` | ✓ **Complete** — buffered reader/writer with `fr_new()`, `fr_read()`, `fw_new()`, `fw_write_byte()`, `fw_write_bytes()`, `fw_flush()`, `fw_done()` |
| `bitstream.c` | ✓ **Complete** — full reader + writer (MSB-first bit packing, auto-flush) |
| `metric.c` | ✓ **Complete** — frequency counting from file |
| `node.c` | ⚠️ **Stub** — `new_node()` returns NULL, `delete_node()` frees but doesn't recurse |
| `sort.c` | ⚠️ **Stub** — `sort_nodes_by_weight()` does nothing, `heapsort()` hardcoded to 256 |
| `list.c` | ⚠️ **Partial** — doubly-linked list works, but `delete_list()` doesn't free nodes, `insert_node()` logic has issues |
| `tree.c` | ⚠️ **Partial** — `new_tree()` works, `delete_tree()` works, `new_tree_from_metric()` has bugs (uses static array but doesn't properly manage parent nodes), `generate_codes()` returns 0, `free_tree_nodes()` has broken sentinel |
| `compress.c` | ⚠️ **Partial** — `compress_file()` full call chain, but `write_header()` and `compress_data()` return 0 (no-op) |
| `decompress.c` | ⚠️ **Partial** — `decompress_file()` full call chain, but `read_header()` returns -1, `decompress_data()` returns 0 (no-op), `reconstruct_tree_from_codes()` returns NULL |

### Build System

- **CMakeLists.txt**: Primary build system (CMake-based)
- **Makefile**: Wrapper that invokes CMake

### Test Files

| File | Status |
|------|--------|
| `tests/test_compress.c` | ⚠️ Calls `compress_file()` and `decompress_file()` but expects them to return `int` (they currently return `void`) |
| `tests/test_helpers.h` | ✓ Complete — temp file creation, file comparison, test macros |
| `tests/test_bitstream.c` | Exists (unseen in scan) |
| `tests/test_file_reader.c` | Exists (unseen in scan) |
| `tests/test_file_writer.c` | Exists (unseen in scan) |
| `tests/test_list.c` | Exists (unseen in scan) |
| `tests/test_tree.c` | Exists (unseen in scan) |

---

## 3. Next Steps

### Phase 1 — Core Data Functions (highest priority)

| Function | File | Status | Description |
|----------|------|--------|-------------|
| `new_node()` | `src/data_structures/node.c` | ⚠️ Stub | Allocate & initialize a node with given byte value and weight |
| `sort_nodes_by_weight()` | `src/utils/sort.c` | ⚠️ Stub | Call heapsort on the first `count` elements of the array (currently a no-op) |
| `generate_codes()` | `src/data_structures/tree.c` | ⚠️ Stub | Traverse Huffman tree (DFS), assign 0/1 codes to leaves, fill `codes[]` and `code_lengths[]`, return symbol count |
| `free_tree_nodes()` | `src/data_structures/tree.c` | ⚠️ Partial | Fix broken sentinel check; properly free parent nodes (static array nodes don't need freeing) |

### Phase 2 — Header I/O

| Function | File | Status | Description |
|----------|------|--------|-------------|
| `write_header()` | `src/core/compress.c` | ⚠️ Stub | Write `num_symbols` (4 bytes LE), then for each symbol: byte value + code length. Use `fw_write_bytes()`. |
| `read_header()` | `src/core/decompress.c` | ⚠️ Stub | Read header format, fill `codes[]` and `code_lengths[]`, return symbol count. Use `fr_read()`. |

### Phase 3 — Compression & Decompression Data Paths

| Function | File | Status | Description |
|----------|------|--------|-------------|
| `compress_data()` | `src/core/compress.c` | ⚠️ Stub | Create `bsw_new(output_fd)`, loop over input bytes, call `bsw_write_bits(code, length)` for each, flush at end. |
| `decompress_data()` | `src/core/decompress.c` | ⚠️ Stub | Create `bs_new(input_fd)`, traverse tree bit-by-bit until leaf reached, write decoded byte via `fw_write_byte()`. |
| `reconstruct_tree_from_codes()` | `src/core/decompress.c` | ⚠️ Stub | Build canonical Huffman tree from code lengths (Fischer-Jaffe or similar algorithm). |

### Phase 4 — Bug Fixes

| Issue | File | Description |
|-------|------|-------------|
| `heapsort()` hardcoded to 256 | `src/utils/sort.c` | Should accept `count` parameter |
| `new_tree_from_metric()` uses static array | `src/data_structures/tree.c` | Parent nodes are stored in static array but `free_tree_nodes()` can't free them properly. Consider dynamic allocation for parent nodes. |
| `insert_node()` logic broken | `src/data_structures/list.c` | `trav->left` reference is wrong; should be `trav->left->right = n` |
| `compress_file()` / `decompress_file()` return `void` | `src/core/compress.c`, `decompress.c` | Tests expect `int` return value (0 = success, -1 = error) |
| `delete_list()` doesn't free nodes | `src/data_structures/list.c` | Only frees the list struct, not the nodes |

### Phase 5 — Polish & Edge Cases

- Handle single-symbol input (tree with only one leaf node)
- Validate header integrity on decompression
- Add progress/logging output options
- Write unit tests for `bitstream` reader/writer, `generate_codes`, and tree reconstruction
- Address any compiler warnings from `-Wall -Wextra`

---

## 4. Key Design Decisions

1. **MSB-first bit ordering** — bits are written/read starting from the most significant bit of each byte (consistent with standard practices).
2. **Buffered I/O** — both reader and writer use a configurable buffer size (default 4096) to minimize system calls.
3. **Static node array in tree** — `tree->nodes[512]` is allocated as part of the `tree` struct. Only internal parent nodes need dynamic allocation, but the current implementation stores them in the static array which creates a `free_tree_nodes()` problem.
4. **Header format** — 4-byte LE integer for symbol count, followed by pairs of (byte value: 1 byte, code length: 1 byte) for each distinct symbol. This is compact and easy to parse.
5. **Error handling** — every function returns a status indicator (`NULL` on allocation failure, `-1` on I/O error). The top-level `compress_file()` / `decompress_file()` clean up all resources before returning.

---

## 5. File Index

```
include/
├── core/
│   ├── compress.h      ✓ complete (compress_file, write_header, compress_data)
│   └── decompress.h    ✓ complete (decompress_file, reconstruct_tree_from_codes)
├── data_structures/
│   ├── bitarray.h      ✓ complete
│   ├── bitstream.h     ✓ complete (reader + writer)
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
│   ├── compress.c      ⚠️ partial (compress_file done, write_header + compress_data stubs)
│   └── decompress.c    ⚠️ partial (decompress_file done, read_header + decompress_data + reconstruct_tree stubs)
├── data_structures/
│   ├── bitstream.c     ✓ complete
│   ├── list.c          ⚠️ partial (delete_list broken, insert_node has issues)
│   ├── node.c          ⚠️ stub (new_node returns NULL)
│   └── tree.c          ⚠️ partial (generate_codes returns 0, free_tree_nodes broken)
├── io/
│   └── file_io.c       ✓ complete
├── main.c              — exists (unseen in scan)
└── utils/
    ├── metric.c        ✓ complete
    └── sort.c          ⚠️ stub (sort_nodes_by_weight is no-op, heapsort hardcoded)

tests/
├── test_compress.c     ⚠️ expects int return from compress/decompress functions
├── test_helpers.h      ✓ complete
├── test_bitstream.c    — exists (unseen in scan)
├── test_file_reader.c  — exists (unseen in scan)
├── test_file_writer.c  — exists (unseen in scan)
├── test_list.c         — exists (unseen in scan)
├── test_tree.c         — exists (unseen in scan)
└── test_utils.c        — exists (unseen in scan)

Makefile                ✓ CMake wrapper
CMakeLists.txt          — primary build system (unseen in scan)
```
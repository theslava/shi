# Agent.md — Huffman Compression Project

## 1. Plan

The goal is to implement a complete **Huffman compression/decompression** tool in C with the following architecture:

| Module | Responsibility |
|--------|---------------|
| `file_reader.c / file_writer.h` | Low-level buffered I/O (read + write) |
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
Input file → fr_read() → metric (frequency counts)
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

| File | Changes |
|------|---------|
| `file_reader.h` | Added `fr_wd` struct + writer API: `fw_new`, `fw_write_byte`, `fw_write_bytes`, `fw_flush`, `fw_done` |
| `file_writer.h` | **New file** — re-exports the writer API for convenience |
| `node.h` | Added `new_node()`, `delete_node()` stubs |
| `sort.h` | Added `sort_nodes_by_weight(node**, int)` stub |
| `list.h` | Added `count` field, `new_list()`, `delete_list()`, `list_size()`, `list_get_head()`, `list_append()` |
| `tree.h` | Added `new_tree()`, `generate_codes(tree*, unsigned int[256], int[256])`, `free_tree_nodes(node*)` stubs |
| `bitarray.h` | Added `ba_write_to_file(bitarray*, fr_fd*)` stub; included `file_reader.h` |
| `bitstream.h` | Added full writer API: `bitstream_writer` struct, `bsw_new`, `bsw_write_bit`, `bsw_write_bits`, `bsw_flush`, `bsw_done` |
| `compress.h` | Added internal helpers: `write_header()`, `read_header()`, `compress_data()`, `decompress_data()` |
| `decompress.h` | Added `reconstruct_tree_from_codes(const unsigned int[256], const int[256], int)` stub |

### Implementation Files (`*.c`)

| File | Changes |
|------|---------|
| `file_reader.c` | Full writer implementation: buffered writes, flush on full buffer, direct write for large data |
| `bitstream.c` | Full writer implementation: MSB-first bit packing, auto-flush when byte is full |
| `node.c` | Added `new_node()` and `delete_node()` stubs |
| `sort.c` | Added `sort_nodes_by_weight()` stub (calls heapsort internally) |
| `list.c` | Full doubly-linked sorted list with size tracking, safe insertion/removal |
| `tree.c` | Added `new_tree()`, `generate_codes()` stub, `free_tree_nodes()` stub; fixed tree building to use `sort_nodes_by_weight` |
| `bitarray.c` | Fixed null checks in `ba_destroy()`, added `ba_write_to_file()` stub |
| `compress.c` | **Full call chain** with error handling: open input → metric → tree → codes → write header → compress data → flush/close → cleanup |
| `decompress.c` | **Full call chain** with error handling: open input → read header → reconstruct tree → decompress data → flush/close → cleanup |

### Build System

- **Makefile**: Added `bitarray.c`, `list.c` to SRCS; added new headers to HEADERS.

---

## 3. Next Steps

### Phase 1 — Core Data Functions (highest priority)

| Function | File | Description |
|----------|------|-------------|
| `new_node()` | `node.c` | Allocate & initialize a node with given byte value and weight |
| `sort_nodes_by_weight()` | `sort.c` | Call heapsort on the first `count` elements of the array |
| `generate_codes()` | `tree.c` | Traverse Huffman tree (DFS), assign 0/1 codes to leaves, fill `codes[]` and `code_lengths[]`, return symbol count |

### Phase 2 — Header I/O

| Function | File | Description |
|----------|------|-------------|
| `write_header()` | `compress.c` | Write `num_symbols` (4 bytes LE), then for each symbol: byte value + code length. Use `fw_write_bytes()`. |
| `read_header()` | `decompress.c` | Read header format, fill `codes[]` and `code_lengths[]`, return symbol count. Use `fr_read()`. |

### Phase 3 — Compression & Decompression Data Paths

| Function | File | Description |
|----------|------|-------------|
| `compress_data()` | `compress.c` | Create `bsw_new(input_fd)`, loop over input bytes, call `bsw_write_bits(code, length)` for each, flush at end. |
| `decompress_data()` | `decompress.c` | Create `bs_new(input_fd)`, traverse tree bit-by-bit until leaf reached, write decoded byte via `fw_write_byte()`. |
| `reconstruct_tree_from_codes()` | `decompress.c` | Build canonical Huffman tree from code lengths (Fischer-Jaffe or similar algorithm). |

### Phase 4 — Polish & Edge Cases

- Handle single-symbol input (tree with only one leaf node)
- Validate header integrity on decompression
- Add progress/logging output options
- Write unit tests for `bitstream` reader/writer, `generate_codes`, and tree reconstruction
- Address any compiler warnings from `-Wall -Wextra`

---

## 4. Key Design Decisions

1. **MSB-first bit ordering** — bits are written/read starting from the most significant bit of each byte (consistent with standard practices).
2. **Buffered I/O** — both reader and writer use a configurable buffer size (default 4096) to minimize system calls.
3. **Static node array in tree** — `tree->nodes[512]` is allocated on the heap as part of the `tree` struct, not dynamically per-node. Only internal parent nodes that need dynamic allocation should use `malloc`.
4. **Header format** — 4-byte LE integer for symbol count, followed by pairs of (byte value: 1 byte, code length: 1 byte) for each distinct symbol. This is compact and easy to parse.
5. **Error handling** — every function returns a status indicator (`NULL` on allocation failure, `-1` on I/O error). The top-level `compress_file()` / `decompress_file()` clean up all resources before returning.

---

## 5. File Index

```
include/
├── bitarray.h      ✓ updated (ba_write_to_file stub)
├── bitstream.h     ✓ updated (full writer API)
├── compress.h      ✓ updated (internal helpers declared)
├── decompress.h    ✓ updated (reconstruct_tree_from_codes stub)
├── file_reader.h   ✓ updated (fr_wd struct + writer API)
├── file_writer.h   ✓ created (writer re-export header)
├── list.h          ✓ updated (count field, new helpers)
├── metric.h        — no changes needed
├── node.h          ✓ updated (new_node, delete_node stubs)
├── sort.h          ✓ updated (sort_nodes_by_weight stub)
└── tree.h          ✓ updated (generate_codes, free_tree_nodes stubs)

*.c
├── bitarray.c      ✓ updated (null checks, ba_write_to_file stub)
├── bitstream.c     ✓ updated (full writer implementation)
├── compress.c      ✓ updated (full call chain with error handling)
├── decompress.c    ✓ updated (full call chain with error handling)
├── file_reader.c   ✓ updated (full writer implementation)
├── list.c          ✓ updated (doubly-linked sorted list)
├── main.c          — no changes needed
├── metric.c        — no changes needed
├── node.c          ✓ updated (new_node, delete_node stubs)
├── sort.c          ✓ updated (sort_nodes_by_weight stub)
└── tree.c          ✓ updated (generate_codes, free_tree_nodes stubs)

Makefile            ✓ updated (added new .c and .h files)
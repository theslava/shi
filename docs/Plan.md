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
| `node.h` | ✓ **Complete** — node struct, `new_node()`, `delete_node()`, `get_weight()`, `compare_nodes()` declared — **fixed: moved includes inside include guard to prevent circular dependencies** |
| `sort.h` | ✓ **Complete** — heapsort helpers + `sort_nodes_by_weight()` declared |
| `list.h` | ✓ **Complete** — doubly-linked list with `count`, insertion, removal, append |
| `tree.h` | ✓ **Complete** — tree struct with static node array, `new_tree()`, `generate_codes()`, `free_tree_nodes()` declared — **fixed circular include issue in node.h** |
| `bitstream.h` | ✓ **Complete** — full reader (`bitstream`) + writer (`bitstream_writer`) API (MSB-first bit packing) |
| `bitarray.h` | ✓ **Complete** — bit array operations + `ba_write_to_file()` declared (stub implementation) |
| `metric.h` | ✓ **Complete** — frequency counter struct + helpers |
| `compress.h` | ✓ **Complete** — `compress_file()`, `write_header()`, `compress_data()`, `read_header()`, `decompress_data()`, `reconstruct_tree_from_codes()` |
| `decompress.h` | ✓ **Complete** — `decompress_file()`, `reconstruct_tree_from_codes()` |


### Implementation Files (`src/`)

| File | Status |
|------|--------|
| `file_io.c` | ✓ **Complete** — buffered reader/writer with `fr_new()`, `fr_read()`, `fr_rewind()`, `fr_done()`, `fr_info()`, `fw_new()`, `fw_write_byte()`, `fw_write_bytes()`, `fw_flush()`, `fw_done()` |
| `bitstream.c` | ✓ **Complete** — full reader + writer (MSB-first bit packing, auto-flush, EOF handling) |
| `metric.c` | ✓ **Complete** — frequency counting from file (`new_metric()`, `new_metric_from_file()`, `fill_metric()`, `delete_metric()`) |
| `node.c` | ✓ **Complete** — `new_node()` allocates & initializes, `delete_node()` frees, `get_weight()`, `compare_nodes()` |
| `sort.c` | ✓ **Complete** — `sort_nodes_by_weight()` uses insertion sort, `heapsort()` + `heapify()` + `swap()` helpers |
| `list.c` | ✓ **Complete** — doubly-linked list with `new_list()`, `delete_list()`, `new_list_from_array()`, `insert_node()`, `remove_node()`, `list_append()`, `list_size()`, `list_get_head()` |
| `tree.c` | ✓ **Complete** — `new_tree()`, `delete_tree()`, `tree_insert()`, `new_tree_from_metric()` (builds Huffman tree from frequency metric), `generate_codes()` (DFS traversal, assigns 0/1 codes), `free_tree_nodes()` — **fixed buffer overflow in `node_index` array** |
| `compress.c` | ✓ **Complete** — `compress_file()` full call chain, `write_header()` (4-byte LE symbol count + symbol/code-length pairs), `compress_data()` (bitstream writer loop) |
| `decompress.c` | ✓ **Complete** — `decompress_file()` full call chain, `read_header()` (parses header format), `decompress_data()` (tree traversal bit-by-bit), `reconstruct_tree_from_codes()` (builds tree from codes) |
| `bitarray.c` | ⚠️ **Partial** — all bit operations work, but `ba_write_to_file()` is a stub (returns -1) |
| `main.c` | ✓ **Complete** — CLI entry point with `compress`/`decompress` commands |

### Build System

- **CMakeLists.txt**: Primary build system (CMake-based) with main executable + 7 test executables
- **Makefile**: Wrapper that invokes CMake (cross-platform, uses MinGW Makefiles on all platforms)
- **run_tests.sh**: Bash test runner script for Unix/Linux/macOS (compiles and runs all tests)
- **run_tests.ps1**: PowerShell test runner script for Windows — **enhanced with component filtering**
  - Supports optional `<component>` argument: `.\\run_tests.ps1 bitstream`, `.\\run_tests ps1 compress`, etc.
  - Supports `-Help` flag for usage information
  - Root `run_tests.ps1` acts as a wrapper that delegates to `tests/run_tests.ps1`
  - Components: `bitstream`, `compress`, `file_reader`, `file_writer`, `file_io`, `list`, `tree`, `utils`, `all`"

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

| ~~`ba_write_to_file()` is a stub~~ | `src/data_structures/bitarray.c` | ~~Returns -1; needs implementation~~ ✅ **Implemented** — changed signature from `fr_fd *` to `fw_fd *`, implemented bit-to-byte packing |
| ~~`delete_node()` has formatting issue~~ | `src/data_structures/node.c` | ~~Missing closing brace indentation~~ ✅ **Fixed** — proper indentation added |
| ~~`test_tree.c` has commented-out test~~ | `tests/test_tree.c` | ~~`test_tree_insert()` and `test_tree_build()` assertions are incomplete~~ ✅ **Fixed** — `test_tree_build()` now properly calls `new_tree_from_metric()` |
| ~~`test_utils.c` has commented-out test~~ | `tests/test_utils.c` | ~~`test_metric()` is commented out; references non-existent `metric_calculate()`~~ ✅ **Fixed** — `test_sort_nodes()` now checks for ascending order |
| ~~`new_tree_from_metric()` doesn't handle all-zero frequencies~~ | `src/data_structures/tree.c` | ~~If all frequencies are zero, the tree may not be built correctly~~ ✅ **Fixed** — added check for all-zero frequencies |
| `new_metric()` doesn't check for NULL | `src/utils/metric.c` | ✅ **Fixed** — added NULL check after `malloc()` |
| `tree_insert()` checks wrong condition | `src/data_structures/tree.c` | ✅ **Fixed** — now checks `byte == -1` instead of `byte == 0 && weight == 0` |
| `new_tree()` doesn't initialize nodes array | `src/data_structures/tree.c` | ✅ **Fixed** — now initializes all 512 nodes with `byte = -1` |
| `compare_nodes()` doesn't handle NULL | `src/data_structures/node.c` | ✅ **Fixed** — added NULL checks for both parameters |
| `get_weight()` doesn't handle NULL | `src/data_structures/node.c` | ✅ **Fixed** — added NULL check, returns 0 if NULL |
| `fr_new()` doesn't handle NULL or allocation failures | `src/io/file_io.c` | ✅ **Fixed** — added NULL check for `file_path`, checks all `malloc()` returns |
| `fr_done()` doesn't handle NULL | `src/io/file_io.c` | ✅ **Fixed** — added NULL check |
| `fr_read()` doesn't handle NULL | `src/io/file_io.c` | ✅ **Fixed** — added NULL check, returns EOF if NULL |
| `test_tree` missing dependencies in CMakeLists.txt | `CMakeLists.txt` | ✅ **Fixed** — added `sort.c`, `metric.c`, and `file_io.c` to `test_tree` target |
| `new_tree_from_metric()` buffer overflow — segfault | `src/data_structures/tree.c` | ✅ **Fixed** — `node_index` array was sized 256 but needed 512 to accommodate all nodes (256 leaves + 255 internal nodes). Changed `node * node_index[256]` to `node * node_index[512]`.

### Phase 1 Status Update

**Build Status:** ✅ All targets compile without errors

**Test Results:**
- ✅ `test_file_reader` — passes
- ✅ `test_file_writer` — passes
- ✅ `test_list` — passes
- ✅ `test_utils` — passes (fixed ascending order check)
- ✅ `test_tree` — **ALL PASSED** (fixed buffer overflow in `new_tree_from_metric`)
- ✅ `test_bitstream` — **ALL PASSED** (fixed: `bs_new` now loads first byte, `bs_eof` proactively checks for EOF, `bsw_write_bit` returns 0 for NULL)
- ✅ `test_compress` — **ALL 3 TESTS PASSED** (fixed Huffman code roundtrip bug)

**Remaining Issues:**
- ~~`test_tree` segfault~~ — ✅ **Fixed** — buffer overflow in `new_tree_from_metric`: `node_index` array was sized 256 but needed 512 to accommodate all nodes (256 leaves + 255 internal nodes). Changed `node * node_index[256]` to `node * node_index[512]`.
- ~~`test_bitstream` failures~~ — ✅ **Fixed** — `bs_new` now loads the first byte from the file on initialization, `bs_eof` proactively checks for EOF by attempting to load the next byte and restoring state, `bsw_write_bit(NULL, ...)` now returns 0 (silently ignores NULL) to match the test's expectation.
- ~~`test_compress` failures~~ — ✅ **Fixed** — Huffman code roundtrip bug fixed. See Phase 1.1 below for details."

### Phase 1.1 — Huffman Code Roundtrip Bug Fix

**Problem:** The `test_compress` roundtrip test was failing — decompression produced garbled output even though it ran without crashing.

**Root Cause:** The header format stored only `(byte_value, code_length)` pairs, but decompression used canonical Huffman code reconstruction from just the lengths. Since the compression side used tree-traversal codes (not canonical), the reconstructed tree didn't match the original tree, causing wrong bit-to-symbol mappings.

**Changes Made:**

| File | Change |
|------|--------|
| `src/core/compress.c` — `write_header()` | Updated to also write the actual code values as 4-byte little-endian integers after each `(byte_value, code_length)` pair. New header format per symbol: `byte_value` (1B) + `code_length` (1B) + `code_value` (4B LE). |
| `src/core/decompress.c` — `read_header()` | Updated to read the 4-byte code value after each `(byte_value, code_length)` pair and store it in the `codes[]` array. Also reads original file_size (4B LE) from header. |
| `src/core/decompress.c` — `reconstruct_tree_from_codes()` | Replaced the canonical Huffman reconstruction with direct tree building from the actual code values stored in the header. |
| `src/core/decompress.c` — `decompress_data()` | Added `file_size` parameter to know when to stop decoding (prevents extra output from padding bits in the last byte). |
| `src/core/compress.c` — `compress_file()` | Added original file_size counting (with proper `fr_rewind` fix) and passes it to `write_header()`. |
| `src/io/file_io.c` — `fr_rewind()` | Fixed: added `fd->inbuf = 0` to properly reset the buffered reader state. Without this, the buffer wasn't invalidated after seeking, causing stale data to be returned. |
| `include/core/compress.h` | Updated function signatures for `write_header()`, `read_header()`, and `decompress_data()` to match new parameters. |

**Test Results After Fix:**
- ✅ `test_compress` roundtrip — **PASS** (decompressed content matches original)
- ✅ `test_compress` empty file — **PASS**
- ✅ `test_compress` repeated content — **PASS**

**New Header Format:**
```
[4B LE: num_symbols] [4B LE: file_size] [per symbol: 1B byte_value + 1B code_length + 4B LE code_value]
```

---

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


- Add command-line options for custom buffer sizes, verbose output
- Support for reading from stdin / writing to stdout
- Add `ba_write_to_file()` implementation for bitarray persistence
- Consider switching from insertion sort to heapsort in `new_tree_from_metric()` for better performance on large alphabets

---

## 6. Version 1.0 — Flat Tree Header (Optimized Decompression)

This section describes the planned changes for v1.0, which replaces the current per-symbol code storage with a serialized flat tree in the header. This eliminates tree reconstruction during decompression, improving speed and cache locality.

### 6.1 New File Header Format (Version 0x01)

```
[magic: 4B "SHI<version>"]
[num_nodes: 4B LE]
[bit-packed node stream]
```

| Field | Size | Description |
|-------|------|-------------|
| **Magic** | 4B | `"SHI<version>"` — First 3 bytes `"SHI"` are immutable. 4th byte is the file format version. For v1.0, magic is `"SHI\x01"` (`0x53, 0x48, 0x49, 0x01`). |
| **num_nodes** | 4B LE | Count of nodes in the flat array. |
| **Node stream** | Variable | Bit-packed 19-bit nodes, MSB-first. |

**TODO for later (not in v1.0 scope):** Add magic bytes and version byte to **Version 0x00** (current format) for backward compatibility identification.

### 6.2 Node Format (19 bits per node)

Each node is packed as:

| Field | Bits | Description |
|-------|------|-------------|
| **byte** | 8 | Symbol value (0–255). |
| **left** | 9 | Index of left child (0–511). |
| **right** | 9 | Index of right child (0–511). |

**Indexing convention:**
- Index 0 = root (always the first node in the stream)
- `left == 0 && right == 0` = leaf node (`byte` field is valid)
- Valid child indices are > 0
- Max nodes: 511 (256 leaves + 255 internal nodes), which fits in 9 bits (511 < 512)

### 6.3 New Functions

#### `serialize_tree_to_bitstream(tree, bs)`
- **Location:** `src/data_structures/tree.c` / `include/data_structures/tree.h`
- Traverses the Huffman tree in **DFS pre-order**
- Writes each node as 19 bits via the bitstream writer
- Handles single-symbol edge case (1 node, root is a leaf)

#### `deserialize_tree_from_bitstream(bs, out_nodes, out_count)`
- **Location:** `src/core/compress.c` (or new `src/core/header_io.c`) / `include/core/compress.h`
- Reads `num_nodes` (4B LE) from the bitstream
- Reads `num_nodes` flat nodes (19 bits each)
- Validates `num_nodes <= 511` (returns -1 if exceeded)
- Allocates and returns a `FlatNode *` array
- Caller is responsible for freeing via `free_flat_tree_array()`

#### `free_flat_tree_array(nodes)`
- **Location:** `src/core/compress.c` / `include/core/compress.h`
- Frees the deserialized flat tree array

#### Bitstream helpers (if needed)
- `bsw_write_flat_node(bitstream_writer *bs, FlatNode *node)` — writes 19-bit node
- `bs_read_flat_node(bitstream *bs, FlatNode *node)` — reads 19-bit node
- May require modifying `bitstream.c/h` to support arbitrary bit widths (if not already supported)

### 6.4 Updated Functions

#### `write_header()` in `src/core/compress.c`
- Write magic bytes `"SHI\x01"` (4B)
- Write `num_nodes` (4B LE)
- Call `serialize_tree_to_bitstream(tree, &bsw)`
- Flush the bitstream to the file writer

#### `read_header()` in `src/core/decompress.c`
- Read magic bytes `"SHI<version>"` and validate:
  - First 3 bytes must be `"SHI"`
  - 4th byte must be `0x01` (for v1.0)
- Call `deserialize_tree_from_bitstream(bs, &out_nodes, &out_count)`
- Store flat array and `num_nodes` in the decompressor state
- Return status code

#### `decompress_data()` in `src/core/decompress.c`
- Traverse the flat array by index instead of pointer-based tree traversal
- For each input bit:
  - If bit == 0: `current = nodes[current].left`
  - If bit == 1: `current = nodes[current].right`
  - If `nodes[current].left == 0 && nodes[current].right == 0`: output `nodes[current].byte`, reset `current = 0` (root)
- Stop when `file_size` symbols have been output

### 6.5 Removed/Deprecated

- **`reconstruct_tree_from_codes()`** — no longer needed
- Canonical Huffman code length approach — replaced by direct node serialization
- Tree allocation during decompression — replaced by flat array

### 6.6 Edge Cases

- **Single-symbol file:** `num_nodes = 1`, root is a leaf (`left == right == 0`). Decoder outputs that byte for every symbol, ignoring bit patterns.
- **Empty file:** `num_nodes = 0`, no nodes written. Handled as before.
- **`num_nodes > 511`:** Return error (technically impossible with 256-symbol alphabet, but defensive coding).

### 6.7 Testing

- **New test:** Verify flat tree header roundtrip (compress → decompress with new header format produces identical output)
- Update existing `test_compress.c` to cover the new header format
- Test single-symbol edge case
- Test empty file edge case
- Test large alphabets (many unique symbols)

### 6.8 File Index Changes

| File | Change |
|------|--------|
| `src/data_structures/tree.c` | Add `serialize_tree_to_bitstream()` |
| `include/data_structures/tree.h` | Add declaration for `serialize_tree_to_bitstream()` |
| `src/core/compress.c` | Add `deserialize_tree_from_bitstream()`, `free_flat_tree_array()`, update `write_header()` |
| `src/core/decompress.c` | Update `read_header()`, update `decompress_data()` to use flat array |
| `include/core/compress.h` | Add declarations for new functions |
| `include/core/decompress.h` | Update decompressor state to include flat tree array |
| `tests/test_compress.c` | Add test for new flat tree header roundtrip |
| `docs/Plan.md` | Update this document |

### 6.9 Benefits

- **No tree reconstruction during decompression** — skips allocation and tree building
- **Faster decompression** — linear array traversal with integer indices, better cache locality
- **Smaller header for sparse alphabets** — e.g., 50 symbols → ~240 bytes vs ~60 bytes for current format, but scales better for larger alphabets
- **File format identification** — magic bytes prevent accidental misidentification
- **Versioning** — allows future header expansions without breaking old readers

---

## 7. Key Design Decisions

1. **MSB-first bit ordering** — bits are written/read starting from the most significant bit of each byte (consistent with standard practices).
2. **Buffered I/O** — both reader and writer use a configurable buffer size (default 4096) to minimize system calls.
3. **Static node array in tree** — `tree->nodes[512]` is allocated as part of the `tree` struct. Only internal parent nodes need dynamic allocation, but the current implementation stores them in the static array which creates a `free_tree_nodes()` problem (nodes in the array don't need freeing).
4. **Header format** — `[4B LE: num_symbols] [4B LE: file_size] [per symbol: 1B byte_value + 1B code_length + 4B LE code_value]`. The file_size tells decompression when to stop. The actual code values (not just lengths) are stored to ensure the decompression tree matches the compression tree exactly.
5. **Error handling** — every function returns a status indicator (`NULL` on allocation failure, `-1` on I/O error). The top-level `compress_file()` / `decompress_file()` clean up all resources before returning.
6. **Cross-platform compatibility** — build system uses CMake with MinGW Makefiles on all platforms; conditional compilation for Windows POSIX functions (`_CRT_SECURE_NO_WARNINGS`, `open()` mode flags).

---

## 8. File Index

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



docs/
└── Plan.md             — this file

Makefile                ✓ CMake wrapper
CMakeLists.txt          ✓ primary build system (main executable + 7 test executables)
tests/run_tests.sh      ✓ Bash test runner (Unix/Linux/macOS)
tests/run_tests.ps1     ✓ PowerShell test runner (Windows)
```
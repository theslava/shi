# Roadmap

## Phase 1 — Bug Fixes & Polish (Completed)

All core functionality implemented and tested. See [Changelog](docs/Changelog.md) for details.

## Phase 2 — Edge Cases & Robustness (Completed) ✅

All Phase 2 items completed. See [Changelog](docs/Changelog.md) for details.

### Phase 2.1 — Codebase Restructuring

- ✅ All headers refactored: consistent formatting, documented APIs
- ✅ All source files refactored: improved types, error handling
- ✅ All tests refactored: improved helpers, better assertions
- ✅ All 7 test suites passing

### Phase 2.2 — Edge Cases, Robustness & Code Quality

- ✅ Added `--verbose` flag for progress/logging output
- ✅ Very large files already handled — file is streamed byte-by-byte via buffered reader, never loaded entirely into memory
- ✅ GCC produces 0 warnings with `-Wall -Wextra -Wpedantic` under C23 (Clang 22 produces 0 warnings under C23; 2 warnings under pre-C23 due to K&R `()` deprecation)
- ✅ Single-symbol input already tested — `test_compress_single_symbol` in `tests/test_compress.c` (50 bytes of 'X', roundtrip verified)
- ✅ Header integrity validation on decompression — tested with 6 corrupted-file scenarios: bad magic, truncated header, num_symbols=0, num_symbols out of range, truncated data, empty file (all return error)

### Phase 2.3 — License & Standards

- ✅ GPL → public domain (Unlicense) across all source files
- ✅ C99 → C23 standard migration
- ✅ K&R `()` → `void` fix for Clang compatibility
- ✅ Warning analysis documented: GCC 0 warnings, Clang 2 warnings (pre-C23 only)

## Phase 3 — Testing Improvements

- [ ] Write unit tests for `generate_codes()` specifically
- [ ] Write unit tests for `reconstruct_tree_from_codes()` specifically
- [ ] Add tests for edge cases: empty file, single-byte file, all-same-byte file, binary data
- [ ] Add integration tests with known-good compressed output

## Phase 4 — Future Enhancements (Out of Scope for v1)

- [ ] Add command-line options for custom buffer sizes, verbose output
- [ ] Support for reading from stdin / writing to stdout
- [ ] Add `ba_write_to_file()` implementation for bitarray persistence
- [ ] Consider switching from insertion sort to heapsort in `new_tree_from_metric()` for better performance

## Version 1.0 — Flat Tree Header (Optimized Decompression)

This section describes the planned changes for v1.0, which replaces the current per-symbol code storage with a serialized flat tree in the header.

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

### 6.9 Benefits

- **No tree reconstruction during decompression** — skips allocation and tree building
- **Faster decompression** — linear array traversal with integer indices, better cache locality
- **Smaller header for sparse alphabets** — e.g., 50 symbols → ~240 bytes vs ~60 bytes for current format, but scales better for larger alphabets
- **File format identification** — magic bytes prevent accidental misidentification
- **Versioning** — allows future header expansions without breaking old readers

# Changelog

## Phase 1 — Bug Fixes & Polish

### Completed Issues

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
| `new_tree_from_metric()` buffer overflow — segfault | `src/data_structures/tree.c` | ✅ **Fixed** — `node_index` array was sized 256 but needed 512. Changed `node * node_index[256]` to `node * node_index[512]`. |

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
- ✅ `test_compress` roundtrip — **PASS**
- ✅ `test_compress` empty file — **PASS**
- ✅ `test_compress` repeated content — **PASS**

**New Header Format:**
```
[4B LE: num_symbols] [4B LE: file_size] [per symbol: 1B byte_value + 1B code_length + 4B LE code_value]
```

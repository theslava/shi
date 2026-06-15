# Changelog

## Phase 3 — File Format Versioning (Completed) ✅

Established infrastructure for multiple file format versions.

### Changes

| File | Change |
|------|--------|
| `include/core/version.h` | **Added** — version constants (`SHI_CURRENT_VERSION`, `SHI_MAX_VERSION`), magic byte definitions (`SHI_MAGIC_V0`, `shi_magic_v0[]`), per-version entry point declarations (`shi_compress_v0`, `shi_decompress_v0`), `shi_detect_version()` declaration |
| `src/main.c` | **Modified** — added `#include "core/version.h"`, `--version <N>` argument parsing with bounds checking, `switch(version)` compress dispatch, magic byte detection + `switch(detected_version)` decompress dispatch |
| `src/core/compress.c` | **Modified** — added `shi_magic_v0[]` definition, `shi_compress_v0()` wrapper delegating to `compress_file()` |
| `src/core/decompress.c` | **Modified** — added `shi_decompress_v0()` wrapper delegating to `decompress_file()` |

### CLI

```
Usage: shi [--version <N>] <compress|decompress> <input_file> <output_file>

Options:
  --version <N>    Compress/decompress using format version N (default: 0)
  -v, --verbose    Enable verbose output
  -h, --help       Show this help message
```

### File Format

Magic bytes now encode the version in the 4th byte: `"SHI<version>"`

| Version | Magic | Description |
|---------|-------|-------------|
| 0 (0x00) | `SHI\x00` | Current format — per-symbol code storage |
| 1 (0x01) | `SHI\x01` | Planned — flat tree header (see Roadmap) |

---

## Phase 1 — Bug Fixes & Polish (Completed)

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

---

## Phase 2 — Edge Cases & Robustness (Completed) ✅

All Phase 2 items completed. See individual sub-phase sections below.

### Phase 2.1 — Codebase Restructuring (Completed) ✅

**Changes Made:**

| File | Change |
|------|--------|
| `build.ps1` | **Added** — PowerShell build script with build/test/clean/distclean targets, auto-detects CMake generator (Ninja → MSVC → MinGW) |
| `CMakeLists.txt` | **Rewritten** — organized test executables, custom targets (`run-tests`, `run-test-*`, `clean-build`, `distclean`), proper cross-platform flags |
| `Makefile` | **Rewritten** — cross-platform CMake wrapper with `configure`, `build`, `test`, `test-*`, `clean`, `distclean`, `help` targets |
| `scripts/clean.ps1` | **Added** — PowerShell cleanup utility |
| `include/` (all headers) | **Refactored** — consistent formatting, documented APIs, unified naming conventions |
| `src/` (all sources) | **Refactored** — consistent formatting, improved types, better error handling, clearer API design |
| `tests/` (all tests) | **Refactored** — improved test helpers, better assertions, consistent structure |
| `README.md` | **Updated** — build instructions, test runner references, documentation |
| `debug_test.c` | **Removed** — legacy debug file |
| `run_tests.ps1` | **Removed** — replaced by `build.ps1 test` |
| `scripts/build.sh` | **Removed** — replaced by `build.ps1` / `Makefile` |
| `scripts/clean.sh` | **Removed** — replaced by `build.ps1 clean` / `make clean` |
| `tests/CTestConfig.cmake` | **Removed** — unused |
| `tests/CTestCustom.cmake` | **Removed** — unused |
| `tests/run_tests.ps1` | **Removed** — replaced by `build.ps1 test` |
| `tests/run_tests.sh` | **Removed** — replaced by `Makefile test` |

**Test Results After Restructuring:**
- ✅ All 7 test suites passing
- ✅ Build compiles with zero errors
- ✅ Cross-platform build scripts (PowerShell + Makefile)
- ✅ `--verbose` flag support via `VERBOSE=1` (Makefile) / `-ShowVerbose` (PowerShell)

### Phase 2.2 — Build System Consolidation (Completed) ✅

**Goal:** Eliminate redundant build wrappers and adopt CMake as the single documented build system for clarity, maintainability, and cross-platform support.

**Changes Made:**

| File | Change |
|------|--------|
| `CMakeLists.txt` | **Enhanced** — added `project()` version metadata, `build-tests` target, `help` target with full usage guide, improved comments and organization |
| `README.md` | **Rewritten** — comprehensive CMake build instructions, test running guide with all 7 test suites, build options table, install instructions |
| `Makefile` | **Removed** — redundant CMake wrapper (both `build.ps1` and `Makefile` only called `cmake` commands) |
| `build.ps1` | **Removed** — redundant CMake wrapper (~212 lines of code that just called `cmake`, `ctest`, and `Remove-Item`) |
| `scripts/clean.ps1` | **Removed** — duplicate of `build.ps1 clean` functionality |
| `docs/Architecture.md` | **Updated** — build system section reflects CMake-only; removed references to wrapper scripts |
| `docs/Roadmap.md` | **Updated** — removed Phase 2.1 items about wrapper scripts from completed checklist |
| `docs/Changelog.md` | **Updated** — removed Phase 2.1 entries about wrapper scripts from completed checklist |

**Rationale:**

The `Makefile` and `build.ps1` were thin wrappers around CMake — they added no build logic, only convenience targets that called `cmake -B`, `cmake --build`, `cmake -E remove_directory`, and `ctest`. This created ~300 lines of duplicated code to maintain. CMake is the industry-standard build system for C projects and natively supports Windows, Linux, and macOS.

**Result:**

- ✅ Single build system (CMake) — no more wrapper maintenance
- ✅ Full cross-platform support — CMake handles platform detection automatically
- ✅ Better documentation — comprehensive build and test instructions in README
- ✅ Reduced codebase — removed ~360 lines of redundant wrapper scripts
- ✅ All 7 test suites still pass after consolidation

---

## Phase 2.3 — License & Compiler Standards (Completed) ✅

### License Change: GPL → Public Domain

**Rationale:** The code is a straightforward educational implementation of Huffman compression. It contains no novel algorithms, proprietary logic, or commercially significant IP. Released to the public domain as a personal educational curiosity.

**Changes:**

| File | Change |
|------|--------|
| `src/main.c` | Removed 21-line GPL header → `/* Released to the public domain — No rights reserved. */` |
| `src/core/compress.c` | Removed 21-line GPL header → public domain |
| `src/core/decompress.c` | Removed 21-line GPL header → public domain |
| `src/data_structures/tree.c` | Removed 20-line GPL header → public domain |
| `include/core/compress.h` | Removed Emacs mode-line (`/* -*- Mode: C; ... -*- */`) → public domain |
| `include/core/decompress.h` | Removed Emacs mode-line → public domain |
| `include/data_structures/bitarray.h` | Removed 21-line GPL header → public domain |
| `include/data_structures/list.h` | Removed 21-line GPL header → public domain |
| `include/data_structures/tree.h` | Removed 21-line GPL header → public domain |
| `include/io/file_io.h` | Removed 21-line GPL header → public domain |
| `include/utils/metric.h` | Removed 21-line GPL header → public domain |
| `include/utils/sort.h` | Removed 21-line GPL header → public domain |

**Total:** 10 files had GPL headers removed (10 × ~21 lines = ~210 lines removed). 2 files had Emacs mode-line headers removed.

---

### C Standard: C99 → C23

**Target:** GCC `--std=c23` (primary compiler). CMakeLists.txt updated: `CMAKE_C_STANDARD` → `23`.

**Compiler Warning Analysis (all with `-Wall -Wextra -Wpedantic`):**

| Compiler | C99 | C11 | C17 | C23 | Notes |
|----------|-----|-----|-----|-----|-------|
| **GCC 15.2.0 (MinGW)** | 0 | 0 | 0 | **0** | Clean across all standards |
| **Clang 22.1.7 (MinGW)** | 2 | 2 | 2 | **0** | K&R `()` deprecation in pre-C23 modes |

**K&R-style `()` fix:** Changed `metric* new_metric()` → `metric* new_metric(void)` in both the declaration (`include/utils/metric.h`) and definition (`src/utils/metric.c`). This is required because:
- In C17 and earlier, `func()` means "unknown number of arguments" (K&R style), which Clang flags as deprecated.
- In C23, `func()` is a proper prototype meaning "no arguments." Clang no longer flags it.
- GCC never flagged this issue.

**MSVC C4996 warnings** (`strcpy`, `open`, `close`, `read`, `lseek` deprecation): Ignored per decision — these are MSVC-specific and only affect builds with MSVC on Windows. GCC/Clang on Linux do not flag these POSIX functions.

**`#include <malloc.h>`:** Used in 4 headers. Both GCC 15 and Clang 22 accept it under C23 without warnings. It is a common cross-platform extension (POSIX + MSVC). No action needed.

**Test Results:** All 7 test suites pass (100%).

---

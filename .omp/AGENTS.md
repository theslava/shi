# Agent Guidelines — Huffman Compression Project

## Project Goal

Implement a complete **Huffman compression/decompression** tool in C with a focus on robustness, clarity, and efficient bit-level I/O.

## Current Phase

### Phase 5 — Flat Tree Header (Not Started)

Phase 5 — Flat Tree Header (v1.0): replace per-symbol code storage with serialized flat tree in the header for faster decompression.

## Documentation Structure

- **[Architecture](docs/Architecture.md)**: Module responsibilities, data flow, key design decisions, and file index.
- **[Roadmap](docs/Roadmap.md)**: Phased work items, current status, and detailed next steps.
- **[Changelog](docs/Changelog.md)**: Detailed history of bug fixes, changes, and rationale.

## Build Commands

- Configure: `cmake -B build -DCMAKE_BUILD_TYPE=Debug`
- Build: `cmake --build build`
- Release: `cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build`

## Test Commands

- All tests: `ctest --test-dir build --output-on-failure`
- Single suite: `ctest --test-dir build -R <name>` (e.g., `test_compress`)
- Verbose: append `-V` to any ctest command

## Code Style

- **C standard**: C23 (`CMAKE_C_STANDARD 23`)
- **Compiler flags**: `-Wall -Wextra -Wpedantic -g` (GCC/Clang), `/W4` (MSVC)
- **Headers**: Unlicense notice in every source file
- **Naming**: lowercase with underscores throughout
- **Error handling**: return status codes or NULL; no exceptions, no aborts
- **MSVC warnings**: C4996 (`strcpy`, POSIX `open/read/write/close/lseek`) intentionally ignored
- **`<malloc.h>`**: used in headers; accepted by GCC 15 and Clang 22 under C23

## Dev Environment Tips

- Use `.opencode/opencode.json` MCP server (`cmake-mcp`) for build introspection if available

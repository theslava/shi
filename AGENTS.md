# Agent Guidelines — Huffman Compression Project

## Project Goal

Implement a complete **Huffman compression/decompression** tool in C with a focus on robustness, clarity, and efficient bit-level I/O.

## Current Phase

### Phase 3 — File Format Versioning (Completed) ✅

All core functionality, versioning infrastructure, and CLI argument parsing are complete.

## Immediate Next Steps

1. Implement Phase 4 (Flat Tree Header v1.0) — see [Roadmap](docs/Roadmap.md) Section 7
2. Write unit tests for `generate_codes()` and `reconstruct_tree_from_codes()`
3. Add integration tests with known-good compressed output

## Documentation Structure

- **[Architecture](docs/Architecture.md)**: Module responsibilities, data flow, key design decisions, and file index.
- **[Roadmap](docs/Roadmap.md)**: Phased work items, current status, and detailed next steps.
- **[Changelog](docs/Changelog.md)**: Detailed history of bug fixes, changes, and rationale.


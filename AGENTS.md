# Agent Guidelines — Huffman Compression Project

## Project Goal

Implement a complete **Huffman compression/decompression** tool in C with a focus on robustness, clarity, and efficient bit-level I/O.

## Current Phase

### Phase 5 — Flat Tree Header (Not Started)

Phase 5 — Flat Tree Header (v1.0): replace per-symbol code storage with serialized flat tree in the header for faster decompression.

## Immediate Next Steps

1. Implement Phase 5 (Flat Tree Header v1.0) — see [Roadmap](docs/Roadmap.md) Section 5
2. ~~Write unit tests for `generate_codes()` and `reconstruct_tree_from_codes()`~~ ✅ (already in test_generate_codes.c and test_reconstruct_tree.c)
3. ~~Add integration tests with known-good compressed output~~ ✅ (already in test_integration.c)
## Documentation Structure

- **[Architecture](docs/Architecture.md)**: Module responsibilities, data flow, key design decisions, and file index.
- **[Roadmap](docs/Roadmap.md)**: Phased work items, current status, and detailed next steps.
- **[Changelog](docs/Changelog.md)**: Detailed history of bug fixes, changes, and rationale.


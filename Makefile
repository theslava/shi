# ===========================================================================
# shi — Cross-platform Makefile (CMake-based)
#
# Targets:
#   make configure   — Configure the build (CMake)
#   make build       — Build the project
#   make test        — Run all tests
#   make clean       — Remove build directory
#   make distclean   — Remove build directory + test artifacts
#   make help        — Show this help message
#
# Variables:
#   VERBOSE=1  — Show full compiler/linker commands
#   BUILD_TYPE — Debug|Release|RelWithDebInfo|MinSizeRel (default: Debug)
#   GEN        — CMake generator (auto-detected)
#
# Examples:
#   make build
#   make build VERBOSE=1
#   make test
#   make clean
#   make build BUILD_TYPE=Release
# ===========================================================================

.PHONY: all configure build test clean distclean help

# ---------------------------------------------------------------------------
# Defaults
# ---------------------------------------------------------------------------
all: build

BUILD_TYPE ?= Debug
GEN        ?= auto

# ---------------------------------------------------------------------------
# Auto-detect CMake generator
# ---------------------------------------------------------------------------
BUILD_DIR := build

# ---------------------------------------------------------------------------
# Targets
# ---------------------------------------------------------------------------

configure:
	@echo "=== Configuring shi (generator: $(GEN)) ==="
	@cmake -B $(BUILD_DIR) -S . \
		-G "$(GEN)" \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	@echo "=== Configuration complete ==="

build: configure
	@echo "=== Building shi ($(BUILD_TYPE)) ==="
	@cmake --build $(BUILD_DIR) --config $(BUILD_TYPE) $(if $(VERBOSE),--verbose)
	@echo "=== Build complete ==="

test: build
	@echo "=== Running tests ==="
	@cd $(BUILD_DIR) && ctest --output-on-failure -C $(BUILD_TYPE)
	@echo "=== All tests passed ==="

test-%: build
	@echo "=== Running test: $* ==="
	@cd $(BUILD_DIR) && ctest -R "^$*" --output-on-failure -C $(BUILD_TYPE)
	@echo "=== Test $* passed ==="

clean:
	@echo "=== Cleaning build directory ==="
	@cmake -E remove_directory $(BUILD_DIR)
	@echo "=== Clean complete ==="

distclean: clean
	@echo "=== Removing test artifacts ==="
	@cmake -E rm -f tests/test_*.tmp
	@echo "=== Distclean complete ==="

help:
	@echo "=== shi Build System ==="
	@echo ""
	@echo "Targets:"
	@echo "  configure   - Configure with CMake (auto-detected generator)"
	@echo "  build       - Build the project"
	@echo "  test        - Run all tests"
	@echo "  test-<name> - Run a specific test (e.g. make test-bitstream)"
	@echo "  clean       - Remove build directory"
	@echo "  distclean   - Remove build directory and test artifacts"
	@echo "  help        - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  BUILD_TYPE=Debug|Release|RelWithDebInfo|MinSizeRel (default: Debug)"
	@echo "  VERBOSE=1     - Show full compiler commands"
	@echo ""
	@echo "To set a custom generator, run:"
	@echo "  make build GEN=\"MinGW Makefiles\""
	@echo "  make build GEN=\"Ninja\""
	@echo "  make build GEN=\"Unix Makefiles\""

# Cross-platform build using CMake
# This Makefile is a simple wrapper for CMake

.PHONY: all build clean test configure

# Default target
all: build

# Build target
build: configure
	cmake --build build

# Clean target
clean:
	@if exist build rmdir /s /q build

# Test target
test: build

	cd build && ctest --output-on-failure

# Run specific test
test-%: build
	cd build && ctest -R $*

# Configure target
configure:
	cmake -B build -S . -G "MinGW Makefiles"
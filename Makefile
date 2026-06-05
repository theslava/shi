# Cross-platform build using CMake
# This Makefile is a simple wrapper for CMake

.PHONY: all build clean test

# Default target
all: build

# Build target
build:
	cmake -B build -S . -G "MinGW Makefiles"
	cmake --build build

# Clean target
clean:
	@if exist build rmdir /s /q build

# Test target
test: build
	cd build && ctest



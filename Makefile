# Cross-platform Makefile for Windows and Linux with gcc

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99 -I./include

# Source files
SRCS = main.c bitarray.c file_reader.c list.c metric.c node.c sort.c tree.c
# Executable name
EXECUTABLE = shi

# Default target
all: build

# Build target
build: $(EXECUTABLE)

# Link object files to create executable
$(EXECUTABLE): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

# Clean target - works on both Unix-like and Windows systems
clean:
ifeq ($(OS),Windows_NT)
	@del $(EXECUTABLE).exe 2>nul || echo "Cleaned"
	@del $(EXECUTABLE) 2>nul || echo "Cleaned"
else
	@rm -f $(EXECUTABLE) 2>/dev/null || echo "Cleaned"
endif

# Phony targets
.PHONY: all build clean


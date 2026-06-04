# Cross-platform Makefile for Windows and Linux with gcc

# Compiler
CC = gcc

# Compiler flags - add -Iinclude so headers in include/ are found
CFLAGS = -Wall -Wextra -std=c99 -g -Iinclude

# Source files (organized by feature)
SRCS = src/main.c \
       src/core/compress.c src/core/decompress.c \
       src/data_structures/bitarray.c src/data_structures/bitstream.c \
       src/data_structures/list.c src/data_structures/node.c \
       src/data_structures/tree.c \
       src/io/file_reader.c src/io/file_writer.c \
       src/utils/metric.c src/utils/sort.c
OBJS = $(SRCS:.c=.o)

# Header files (organized by feature)
HEADERS = include/core/compress.h include/core/decompress.h \
          include/data_structures/bitarray.h include/data_structures/bitstream.h \
          include/data_structures/list.h include/data_structures/node.h \
          include/data_structures/tree.h \
          include/io/file_reader.h include/io/file_writer.h \
          include/utils/metric.h include/utils/sort.h
TARGET = shi
TEST_SRCS = tests/test_bitstream.c src/io/file_reader.c src/data_structures/bitstream.c
TEST_TARGET = test_bitstream

# Platform detection
RM := $(strip $(shell cmd /c "if "$(OS)"=="Windows_NT" (echo del /Q /F) else (echo rm -f)"))
RUN := $(strip $(shell cmd /c "if "$(OS)"=="Windows_NT" (echo .\) else (echo ./)"))

# Default target
all: build

# Build target
build: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Test target
test: $(TEST_TARGET)
	$(RUN)$(TEST_TARGET) test.txt

# Compile test executable
$(TEST_TARGET): $(TEST_SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SRCS)

# Clean target - works on both Unix-like and Windows systems
clean:
	$(RM) $(OBJS) $(TARGET) $(TEST_TARGET)

# Phony targets
.PHONY: all build clean test


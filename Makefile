# Cross-platform Makefile for Windows and Linux with gcc

# Compiler
CC = gcc

# Compiler flags - add -Iinclude so headers in include/ are found
CFLAGS = -Wall -Wextra -std=c99 -g -Iinclude

# Source files
SRCS = main.c file_reader.c metric.c node.c sort.c tree.c compress.c decompress.c bitstream.c bitarray.c list.c
OBJS = $(SRCS:.c=.o)

# Header files
HEADERS = include/file_reader.h include/metric.h include/node.h include/sort.h \
          include/tree.h include/compress.h include/decompress.h include/bitstream.h \
          include/bitarray.h include/list.h include/file_writer.h

TARGET = compressor
TEST_SRCS = test_bitstream.c file_reader.c bitstream.c
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


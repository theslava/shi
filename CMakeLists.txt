cmake_minimum_required(VERSION 3.15)
project(shi C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
    # Disable POSIX name warnings on Windows
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -g)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files for the main executable
set(MAIN_SOURCES
    src/main.c
    src/core/compress.c
    src/core/decompress.c
    src/data_structures/bitarray.c
    src/data_structures/bitstream.c
    src/data_structures/list.c
    src/data_structures/node.c
    src/data_structures/tree.c
    src/io/file_io.c
    src/utils/metric.c
    src/utils/sort.c
)

# Main executable
add_executable(shi ${MAIN_SOURCES})

# Enable testing
enable_testing()

# Test sources and executables
set(TEST_SOURCES
    tests/test_bitstream.c
    tests/test_compress.c
    tests/test_file_reader.c
    tests/test_file_writer.c
    tests/test_list.c
    tests/test_tree.c
    tests/test_utils.c
)

# Define test executables and their dependencies
add_executable(test_bitstream
    tests/test_bitstream.c
    src/io/file_io.c
    src/data_structures/bitstream.c
)
target_include_directories(test_bitstream PRIVATE ${CMAKE_SOURCE_DIR}/tests)
add_test(NAME test_bitstream COMMAND test_bitstream)

add_executable(test_compress
    tests/test_compress.c
    src/core/compress.c
    src/core/decompress.c
    src/io/file_io.c
    src/data_structures/bitstream.c
    src/data_structures/list.c
    src/data_structures/node.c
    src/data_structures/tree.c
    src/data_structures/bitarray.c
    src/utils/metric.c
    src/utils/sort.c
)
target_include_directories(test_compress PRIVATE ${CMAKE_SOURCE_DIR}/tests)
add_test(NAME test_compress COMMAND test_compress)

add_executable(test_file_reader
    tests/test_file_reader.c
    src/io/file_io.c
)
target_include_directories(test_file_reader PRIVATE ${CMAKE_SOURCE_DIR}/tests)
add_test(NAME test_file_reader COMMAND test_file_reader)

add_executable(test_file_writer
    tests/test_file_writer.c
    src/io/file_io.c
)
target_include_directories(test_file_writer PRIVATE ${CMAKE_SOURCE_DIR}/tests)
add_test(NAME test_file_writer COMMAND test_file_writer)

add_executable(test_list
    tests/test_list.c
    src/data_structures/list.c
    src/data_structures/node.c
)
target_include_directories(test_list PRIVATE ${CMAKE_SOURCE_DIR}/tests)
add_test(NAME test_list COMMAND test_list)

add_executable(test_tree
    tests/test_tree.c
    src/data_structures/tree.c
    src/data_structures/node.c
)
target_include_directories(test_tree PRIVATE ${CMAKE_SOURCE_DIR}/tests)
add_test(NAME test_tree COMMAND test_tree)

add_executable(test_utils
    tests/test_utils.c
    src/utils/metric.c
    src/utils/sort.c
    src/data_structures/node.c
    src/io/file_io.c
)
target_include_directories(test_utils PRIVATE ${CMAKE_SOURCE_DIR}/tests)
add_test(NAME test_utils COMMAND test_utils)

cmake_minimum_required(VERSION 3.15)
project(shi C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

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

# Test sources
set(TEST_BITSTREAM_SOURCES
    tests/test_bitstream.c
    src/io/file_io.c
    src/data_structures/bitstream.c
)

# Test executable - add tests directory to include path
#add_executable(test_bitstream ${TEST_BITSTREAM_SOURCES})
#target_include_directories(test_bitstream PRIVATE ${CMAKE_SOURCE_DIR}/tests)

# Enable testing
#enable_testing()
#add_test(NAME test_bitstream COMMAND test_bitstream test.txt)

# Phony targets (clean)
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMENT "Cleaning build artifacts"
)
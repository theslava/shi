cmake_minimum_required(VERSION 3.15)
project(shi C)

# ---------------------------------------------------------------------------
# Project settings
# ---------------------------------------------------------------------------
set(CMAKE_C_STANDARD          99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Verbose build: pass VERBOSE=1 to make / ninja to see full commands
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# ---------------------------------------------------------------------------
# Compiler flags
# ---------------------------------------------------------------------------
if(MSVC)
    add_compile_options(/W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -g)
endif()

# ---------------------------------------------------------------------------
# Include directory
# ---------------------------------------------------------------------------
include_directories(${CMAKE_SOURCE_DIR}/include)

# ---------------------------------------------------------------------------
# Main executable
# ---------------------------------------------------------------------------
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
add_executable(shi ${MAIN_SOURCES})

# ---------------------------------------------------------------------------
# Testing
# ---------------------------------------------------------------------------
enable_testing()

# ---------------------------------------------------------------------------
# Helper: create a test executable from a name and source list
# ---------------------------------------------------------------------------
function(add_shi_test NAME)
    cmake_parse_arguments(T "" "" "SOURCES" ${ARGN})

    add_executable(${NAME}
        tests/${NAME}.c
        tests/test_helpers.c
        ${T_SOURCES}
    )
    target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/tests)
    add_test(NAME ${NAME} COMMAND ${NAME})
endfunction()

# ---------------------------------------------------------------------------
# Test executables
# ---------------------------------------------------------------------------

add_shi_test(test_bitstream
    SOURCES
        src/data_structures/bitstream.c
        src/io/file_io.c
)

add_shi_test(test_compress
    SOURCES
        src/core/compress.c
        src/core/decompress.c
        src/data_structures/bitstream.c
        src/data_structures/list.c
        src/data_structures/node.c
        src/data_structures/tree.c
        src/data_structures/bitarray.c
        src/utils/metric.c
        src/utils/sort.c
        src/io/file_io.c
)

add_shi_test(test_file_reader
    SOURCES
        src/io/file_io.c
)

add_shi_test(test_file_writer
    SOURCES
        src/io/file_io.c
)

add_shi_test(test_list
    SOURCES
        src/data_structures/list.c
        src/data_structures/node.c
)

add_shi_test(test_tree
    SOURCES
        src/data_structures/tree.c
        src/data_structures/node.c
        src/utils/sort.c
        src/utils/metric.c
        src/io/file_io.c
)

add_shi_test(test_utils
    SOURCES
        src/utils/metric.c
        src/utils/sort.c
        src/data_structures/node.c
        src/io/file_io.c
)

# ---------------------------------------------------------------------------
# Custom targets
# ---------------------------------------------------------------------------

# Clean build directory
add_custom_target(clean-build
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Removing build directory..."
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Full distclean (build + generated test artifacts)
add_custom_target(distclean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E rm -f tests/test_*.tmp
    COMMENT "Removing build directory and test artifacts..."
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Run all tests
add_custom_target(run-tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    COMMENT "Running all tests..."
)

# Run a specific test: make run-test-NAME
function(add_run_test_target NAME)
    add_custom_target(run-test-${NAME}
        COMMAND ${CMAKE_CTEST_COMMAND} -R "^${NAME}$" --output-on-failure
        COMMENT "Running test: ${NAME}..."
    )
endfunction()
add_run_test_target(test_bitstream)
add_run_test_target(test_compress)
add_run_test_target(test_file_reader)
add_run_test_target(test_file_writer)
add_run_test_target(test_list)
add_run_test_target(test_tree)
add_run_test_target(test_utils)

# ---------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------
install(TARGETS shi RUNTIME DESTINATION bin)

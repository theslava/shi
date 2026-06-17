cmake_minimum_required(VERSION 3.15)
project(shi
    VERSION 1.0.0
    DESCRIPTION "Huffman Compression Implementation"
    LANGUAGES C
)

# ---------------------------------------------------------------------------
# Project settings
# ---------------------------------------------------------------------------
set(CMAKE_C_STANDARD          23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Debug build if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type: Debug, Release, RelWithDebInfo, MinSizeRel" FORCE)
endif()

# ---------------------------------------------------------------------------
# Compiler flags — GCC/Clang
# ---------------------------------------------------------------------------
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -g)
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4)
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
    src/cli/args.c
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

add_shi_test(test_decompress_version
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

add_shi_test(test_args
    SOURCES
        src/cli/args.c
)

add_shi_test(test_generate_codes
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

add_shi_test(test_reconstruct_tree
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

add_shi_test(test_integration
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

# ---------------------------------------------------------------------------
# Custom targets
# ---------------------------------------------------------------------------

# Build all tests
add_custom_target(build-tests
    DEPENDS
        test_bitstream
        test_compress
        test_file_reader
        test_file_writer
        test_list
        test_tree
        test_utils
        test_decompress_version
        test_args
        test_generate_codes
        test_reconstruct_tree
        test_integration
    COMMENT "Building all test executables..."
)

# Run all tests (convenience target)
add_custom_target(run-tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG> --parallel
    COMMENT "Running all tests..."
)

# Run a specific test: cmake --build . --target run-test-<name>
function(add_run_test_target NAME)
    add_custom_target(run-test-${NAME}
        COMMAND ${CMAKE_CTEST_COMMAND} -R "^${NAME}$" --output-on-failure -C $<CONFIG>
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
add_run_test_target(test_decompress_version)
add_run_test_target(test_args)
add_run_test_target(test_generate_codes)
add_run_test_target(test_reconstruct_tree)
add_run_test_target(test_integration)

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

# ---------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------
install(TARGETS shi RUNTIME DESTINATION bin)

# ---------------------------------------------------------------------------
# Help target — use via: cmake --build . --target help
# ---------------------------------------------------------------------------
add_custom_target(show-help
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "=== shi Build System ==="
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Build:"
    COMMAND ${CMAKE_COMMAND} -E echo "  cmake -B build -DCMAKE_BUILD_TYPE=Debug    # Configure (default: Debug)"
    COMMAND ${CMAKE_COMMAND} -E echo "  cmake --build build                         # Build"
    COMMAND ${CMAKE_COMMAND} -E echo "  cmake --build build --config Release        # Release build"
    COMMAND ${CMAKE_COMMAND} -E echo "  cmake --build build --verbose               # Verbose build"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Test:"
    COMMAND ${CMAKE_COMMAND} -E echo "  ctest --test-dir build                      # Run all tests"
    COMMAND ${CMAKE_COMMAND} -E echo "  ctest --test-dir build -R test_compress     # Run specific test"
    COMMAND ${CMAKE_COMMAND} -E echo "  ctest --test-dir build --config Debug       # Run all tests"
    COMMAND ${CMAKE_COMMAND} -E echo "  ctest --test-dir build -R test_compress     # Run specific test"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Clean:"
    COMMAND ${CMAKE_COMMAND} -E echo "  cmake --build . --target clean-build        # Remove build directory"
    COMMAND ${CMAKE_COMMAND} -E echo "  cmake --build . --target distclean          # Remove build directory + test artifacts"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Install:"
    COMMAND ${CMAKE_COMMAND} -E echo "  cmake --install build                         # Install to system"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Build type options: Debug, Release, RelWithDebInfo, MinSizeRel"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMENT "Displaying build help..."
)

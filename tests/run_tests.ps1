# Test runner script for shi (Slava's Huffman Implementation)
# This script compiles and runs all test suites.
# Windows PowerShell version
#
# Usage:
#   .\run_tests.ps1                          # Run all tests
#   .\run_tests.ps1 <component>              # Run tests for a specific component
#   .\run_tests.ps1 -help                    # Show help message
#
# Components:
#   bitstream     - Bitstream reader/writer tests
#   compress      - Compression/decompression tests
#   file_reader   - File reader tests
#   file_writer   - File writer tests
#   list          - Linked list tests
#   tree          - Huffman tree tests
#   utils         - Utility functions tests (metric, sort)
#   all           - Run all tests (default)

param(
    [Parameter(Position = 0)]
    [string]$Component = "all",

    [Parameter(Mandatory = $false)]
    [switch]$Help
)

$ErrorActionPreference = "Continue"

# Navigate to the script directory first
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptDir

# Colors for output - use PowerShell built-in color names
$RED = "Red"
$GREEN = "Green"
$YELLOW = "Yellow"
$White = "White"

# Project directories
$SRC_DIR = "..\src"
$INCLUDE_DIR = "..\include"
$BUILD_DIR = "build"

# Common compiler flags
$CFLAGS = "-Wall -Wextra -std=c99 -I$INCLUDE_DIR -g"

# Define all test sources and their dependencies
$ALL_TESTS = @{
    "test_bitstream"   = @{
        Source = "test_bitstream.c"
        Deps   = @("..\src\io\file_io.c", "..\src\data_structures\bitstream.c")
    }
    "test_compress"    = @{
        Source = "test_compress.c"
        Deps   = @("..\src\core\compress.c", "..\src\core\decompress.c",
                   "..\src\io\file_io.c", "..\src\data_structures\bitstream.c",
                   "..\src\data_structures\list.c", "..\src\data_structures\node.c",
                   "..\src\data_structures\tree.c", "..\src\data_structures\bitarray.c",
                   "..\src\utils\metric.c", "..\src\utils\sort.c")
    }
    "test_file_reader" = @{
        Source = "test_file_reader.c"
        Deps   = @("..\src\io\file_io.c")
    }
    "test_file_writer" = @{
        Source = "test_file_writer.c"
        Deps   = @("..\src\io\file_io.c")
    }
    "test_list"        = @{
        Source = "test_list.c"
        Deps   = @("..\src\data_structures\list.c", "..\src\data_structures\node.c")
    }
    "test_tree"        = @{
        Source = "test_tree.c"
        Deps   = @("..\src\data_structures\tree.c", "..\src\data_structures\node.c")
    }
    "test_utils"       = @{
        Source = "test_utils.c"
        Deps   = @("..\src\utils\metric.c", "..\src\utils\sort.c",
                   "..\src\data_structures\node.c", "..\src\io\file_io.c")
    }
}

# Map component names to test names
$COMPONENT_MAP = @{
    "bitstream"     = @("test_bitstream")
    "compress"      = @("test_compress")
    "file_reader"   = @("test_file_reader")
    "file_writer"   = @("test_file_writer")
    "file_io"       = @("test_file_reader", "test_file_writer")
    "list"          = @("test_list")
    "tree"          = @("test_tree")
    "utils"         = @("test_utils")
    "all"           = $ALL_TESTS.Keys | ForEach-Object { $_ }
}

function Show-Help {
    Write-Host "========================================" -ForegroundColor $YELLOW
    Write-Host "  shi Test Runner - Help" -ForegroundColor $YELLOW
    Write-Host "========================================" -ForegroundColor $YELLOW
    Write-Host ""
    Write-Host "Usage:" -ForegroundColor $GREEN
    Write-Host "  .\run_tests.ps1                          Run all tests" -ForegroundColor $White
    Write-Host "  .\run_tests.ps1 <component>              Run tests for a specific component" -ForegroundColor $White
    Write-Host "  .\run_tests.ps1 -help                    Show this help message" -ForegroundColor $White
    Write-Host ""
    Write-Host "Available components:" -ForegroundColor $GREEN
    $COMPONENT_MAP.Keys | Where-Object { $_ -ne "all" } | Sort-Object | ForEach-Object {
        Write-Host "  $_" -ForegroundColor $White
    }
    Write-Host ""
    Write-Host "Examples:" -ForegroundColor $GREEN
    Write-Host "  .\run_tests.ps1" -ForegroundColor $White
    Write-Host "  .\run_tests.ps1 bitstream" -ForegroundColor $White
    Write-Host "  .\run_tests.ps1 file_io" -ForegroundColor $White
    Write-Host "  .\run_tests.ps1 compress" -ForegroundColor $White
    Write-Host ""
}

# Handle help flag
if ($Help) {
    Show-Help
    exit 0
}

# Validate component argument
$validComponents = $COMPONENT_MAP.Keys
if ($Component -notin $validComponents) {
    Write-Host "Error: Invalid component '$Component'" -ForegroundColor $RED
    Write-Host ""
    Write-Host "Valid components are:" -ForegroundColor $Yellow
    $validComponents | Where-Object { $_ -ne "all" } | ForEach-Object {
        Write-Host "  - $_" -ForegroundColor $White
    }
    Write-Host ""
    Write-Host "Run with -help for usage information" -ForegroundColor $Yellow
    exit 1
}

# Get the list of tests to run based on component
if ($Component -eq "all") {
    $testsToRun = $ALL_TESTS.Keys | Sort-Object
} else {
    $testsToRun = $COMPONENT_MAP[$Component]
}

# Display header
Write-Host "========================================" -ForegroundColor $YELLOW
Write-Host "  shi Test Runner" -ForegroundColor $YELLOW
Write-Host "========================================" -ForegroundColor $YELLOW
if ($Component -ne "all") {
    Write-Host "  Component: $Component" -ForegroundColor $White
}
Write-Host "========================================" -ForegroundColor $YELLOW
Write-Host ""

# Create build directory
New-Item -ItemType Directory -Force -Path $BUILD_DIR | Out-Null

$TOTAL_TESTS = $testsToRun.Count
$PASSED = 0
$FAILED = 0
$SKIPPED = 0

foreach ($test_name in $testsToRun) {
    $test_info = $ALL_TESTS[$test_name]
    $test_src = $test_info.Source
    $test_deps = $test_info.Deps
    $test_exec = Join-Path $BUILD_DIR "$test_name.exe"

    Write-Host "Compiling $test_src..." -ForegroundColor $YELLOW


    # Build the compile arguments
    $depArgsArray = @()
    foreach ($dep in $test_deps) {
        $depArgsArray += "`"$dep`""
    }
    $depArgsArray += "-o"
    $depArgsArray += "`"$test_exec`""

    # Compile the test
    Write-Host "  Compiling..." -ForegroundColor $White
    $compileOutput = gcc -Wall -Wextra -std=c99 "-I$INCLUDE_DIR" -g $test_src @depArgsArray 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Compiled successfully" -ForegroundColor $GREEN
    } else {
        Write-Host "Compilation failed" -ForegroundColor $RED
        Write-Host $compileOutput
        $FAILED++
        continue
    }

    # Run the test
    Write-Host "  Running $test_name..." -ForegroundColor $Yellow
    $testOutput = & $test_exec 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  $test_name PASSED" -ForegroundColor $Green
        $PASSED++
    } else {
        Write-Host "  $test_name FAILED" -ForegroundColor $Red
        Write-Host $testOutput
        $FAILED++
    }

    # Clean up executable
    Remove-Item $test_exec -Force -ErrorAction SilentlyContinue
    Write-Host ""
}

# Summary
Write-Host "========================================" -ForegroundColor $YELLOW
Write-Host "  Test Summary" -ForegroundColor $YELLOW
Write-Host "========================================" -ForegroundColor $YELLOW
Write-Host "Total:  $TOTAL_TESTS" -ForegroundColor $White
Write-Host "Passed: $PASSED" -ForegroundColor $Green
Write-Host "Failed: $FAILED" -ForegroundColor $Red
Write-Host "Skipped: $SKIPPED" -ForegroundColor $Yellow
Write-Host "========================================" -ForegroundColor $YELLOW

if ($FAILED -gt 0) {
    Write-Host "Some tests failed!" -ForegroundColor $RED
    exit 1
} else {
    Write-Host "All tests passed!" -ForegroundColor $Green
    exit 0
}

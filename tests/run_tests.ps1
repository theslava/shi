# Test runner script for shi (Slava's Huffman Implementation)
# This script compiles and runs all test suites.
# Windows PowerShell version

$ErrorActionPreference = "Continue"

# Navigate to the script directory first
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptDir

# Colors for output - use PowerShell built-in color names
$RED = "Red"
$GREEN = "Green"
$YELLOW = "Yellow"
$NC = "White"

# Project directories
$SRC_DIR = "..\src"
$INCLUDE_DIR = "..\include"
$BUILD_DIR = "build"

# Test source files
$TEST_SOURCES = @(
    "test_bitstream.c",
    "test_compress.c",
    "test_file_reader.c",
    "test_file_writer.c",
    "test_list.c",
    "test_tree.c",
    "test_utils.c"
)

# Common compiler flags
$CFLAGS = "-Wall -Wextra -std=c99 -I$INCLUDE_DIR -g"

Write-Host "========================================"
Write-Host "  shi Test Runner"
Write-Host "========================================"
Write-Host ""

# Create build directory
New-Item -ItemType Directory -Force -Path $BUILD_DIR | Out-Null

$TOTAL_TESTS = $TEST_SOURCES.Count
$PASSED = 0
$FAILED = 0
$SKIPPED = 0

foreach ($test_src in $TEST_SOURCES) {
    $test_name = $test_src -replace '\.c$', ''
    $test_exec = Join-Path $BUILD_DIR "$test_name.exe"

    Write-Host "Compiling $test_src..." -ForegroundColor $YELLOW

    # Determine which source files to include based on the test
    $test_deps = switch ($test_name) {
        "test_bitstream" { "..\src\io\file_io.c ..\src\data_structures\bitstream.c" }
        "test_compress"  { "..\src\core\compress.c ..\src\core\decompress.c" }
        "test_file_reader" { "..\src\io\file_io.c" }
        "test_file_writer" { "..\src\io\file_io.c" }
        "test_list"      { "..\src\data_structures\list.c ..\src\data_structures\node.c" }
        "test_tree"      { "..\src\data_structures\tree.c ..\src\data_structures\node.c" }
        "test_utils"     { "..\src\utils\metric.c ..\src\utils\sort.c" }
        default          { "" }
    }

    # Compile the test
    $compileCmd = "gcc $CFLAGS `"$test_src`" $test_deps -o `"$test_exec`""
    $compileOutput = Invoke-Expression $compileCmd 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Compiled successfully" -ForegroundColor $GREEN
    } else {
        Write-Host "Compilation failed" -ForegroundColor $RED
        Write-Host $compileOutput
        $FAILED++
        continue
    }

    # Run the test
    Write-Host "Running $test_name..." -ForegroundColor $YELLOW
    $testOutput = Invoke-Expression $test_exec 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host "$test_name PASSED" -ForegroundColor $GREEN
        $PASSED++
    } else {
        Write-Host "$test_name FAILED" -ForegroundColor $RED
        Write-Host $testOutput
        $FAILED++
    }

    # Clean up executable
    Remove-Item $test_exec -Force -ErrorAction SilentlyContinue
    Write-Host ""
}

# Summary
Write-Host "========================================"
Write-Host "  Test Summary"
Write-Host "========================================"
Write-Host "Total:  $TOTAL_TESTS"
Write-Host "Passed: $PASSED" -ForegroundColor $GREEN
Write-Host "Failed: $FAILED" -ForegroundColor $RED
Write-Host "Skipped: $SKIPPED" -ForegroundColor $YELLOW
Write-Host "========================================"

if ($FAILED -gt 0) {
    Write-Host "Some tests failed!" -ForegroundColor $RED
    exit 1
} else {
    Write-Host "All tests passed!" -ForegroundColor $GREEN
    exit 0
}

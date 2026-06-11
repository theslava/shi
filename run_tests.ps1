# Test runner wrapper for shi (Slava's Huffman Implementation)
# This script delegates to the actual test runner in the tests/ directory.
#
# Usage:
#   .\run_tests.ps1                          # Run all tests
#   .\run_tests.ps1 <component>              # Run tests for a specific component
#   .\run_tests.ps1 -help                    # Show help message
#
# For more information, see tests/run_tests.ps1

param(
    [Parameter(Position = 0, ValueFromRemainingArguments = $true)]
    [string[]]$RemainingArgs
)

# Get the directory where this script is located
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$testsScript = Join-Path $scriptDir "tests\run_tests.ps1"

# Check if the actual test runner exists
if (-not (Test-Path $testsScript)) {
    Write-Host "Error: Test runner not found at $testsScript" -ForegroundColor Red
    exit 1
}

# Invoke the actual test runner with all arguments
& $testsScript @RemainingArgs


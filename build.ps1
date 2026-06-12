<#
.SYNOPSIS
    Cross-platform build script for shi (Huffman compression tool).
.DESCRIPTION
    Wrapper around CMake for Windows (PowerShell). Provides build, test,
    clean, and distclean targets with feature parity to the Linux shell script.
.PARAMETER Target
    Build target: build, test, clean, distclean, help (default: build)
.PARAMETER Component
    Test component to run (e.g., bitstream, compress, list, tree, utils)
.PARAMETER BuildType
    CMake build type: Debug, Release, RelWithDebInfo, MinSizeRel
.PARAMETER ShowVerbose
    Show full compiler/linker commands
.EXAMPLE
    .\build.ps1
    .\build.ps1 build
    .\build.ps1 test
    .\build.ps1 test bitstream
    .\build.ps1 clean
    .\build.ps1 distclean
    .\build.ps1 build -BuildType Release
    .\build.ps1 -Verbose
#>
param(
    [Parameter(Position = 0)]
    [ValidateSet("build", "test", "clean", "distclean", "help")]
    [string]$Target = "build",

    [Parameter(Position = 1)]
    [string]$Component = "",

    [Parameter(Mandatory = $false)]
    [ValidateSet("Debug", "Release", "RelWithDebInfo", "MinSizeRel")]
    [string]$BuildType = "Debug",

    [switch]$ShowVerbose
)

$ErrorActionPreference = "Stop"

# --- Helpers ---
function Write-Header {
    param([string]$Msg)
    Write-Host ""
    Write-Host "================================================" -ForegroundColor Cyan
    Write-Host "  $Msg" -ForegroundColor Cyan
    Write-Host "================================================" -ForegroundColor Cyan
    Write-Host ""
}

function Write-Success {
    param([string]$Msg)
    Write-Host "  [OK] $Msg" -ForegroundColor Green
}

function Write-Error-Custom {
    param([string]$Msg)
    Write-Host "  [ERR] $Msg" -ForegroundColor Red
}

function Write-Warning-Custom {
    param([string]$Msg)
    Write-Host "  [WARN] $Msg" -ForegroundColor Yellow
}

function Show-Help {
    Write-Host "Usage: .\build.ps1 <target> [component]" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Targets:" -ForegroundColor Yellow
    Write-Host "  build       - Configure and build the project" -ForegroundColor White
    Write-Host "  test        - Run all tests" -ForegroundColor White
    Write-Host "  test <name> - Run a specific test (e.g., bitstream, compress)" -ForegroundColor White
    Write-Host "  clean       - Remove build directory" -ForegroundColor White
    Write-Host "  distclean   - Remove build directory and test artifacts" -ForegroundColor White
    Write-Host "  help        - Show this help message" -ForegroundColor White
    Write-Host ""
    Write-Host "Parameters:" -ForegroundColor Yellow
    Write-Host "  -BuildType <type>   Debug|Release|RelWithDebInfo|MinSizeRel (default: Debug)" -ForegroundColor White
    Write-Host "  -ShowVerbose        Show full compiler commands" -ForegroundColor White
    Write-Host ""
    Write-Host "Examples:" -ForegroundColor Yellow
    Write-Host "  .\build.ps1" -ForegroundColor White
    Write-Host "  .\build.ps1 build" -ForegroundColor White
    Write-Host "  .\build.ps1 test" -ForegroundColor White
    Write-Host "  .\build.ps1 test bitstream" -ForegroundColor White
    Write-Host "  .\build.ps1 clean" -ForegroundColor White
    Write-Host "  .\build.ps1 build -BuildType Release" -ForegroundColor White
    Write-Host ""
    exit 0
}

# --- Detect CMake generator ---
function Get-CMakeGenerator {
    # Try Ninja first
    if (Get-Command ninja -ErrorAction SilentlyContinue) {
        return "Ninja"
    }
    # Try MSVC (cl.exe via VS Developer Command Prompt)
    if (Get-Command cl -ErrorAction SilentlyContinue) {
        return "Visual Studio 17 2022"
    }
    # Fall back to MinGW Makefiles
    if (Get-Command mingw32-make -ErrorAction SilentlyContinue) {
        return "MinGW Makefiles"
    }
    if (Get-Command make -ErrorAction SilentlyContinue) {
        return "MinGW Makefiles"
    }
    return $null
}

$generator = Get-CMakeGenerator
$buildDir  = "build"

# --- Targets ---
switch ($Target) {
    "build" {
        Write-Header "Configuring shi"
        if (-not $generator) {
            Write-Error-Custom "No CMake generator found. Install Ninja, MSVC, or MinGW."
            exit 1
        }
        Write-Host "  Generator: $generator" -ForegroundColor White
        Write-Host "  Build type: $BuildType" -ForegroundColor White
        Write-Host ""

        cmake -B $buildDir -S . `
            -G $generator `
            -DCMAKE_BUILD_TYPE=$BuildType
        if ($LASTEXITCODE -ne 0) {
            Write-Error-Custom "CMake configuration failed."
            exit 1
        }
        Write-Success "Configuration complete."

        Write-Header "Building shi"
        $cmakeArgs = @("--build", $buildDir, "--config", $BuildType)
        if ($ShowVerbose) { $cmakeArgs += "--verbose" }

        cmake @cmakeArgs
        if ($LASTEXITCODE -ne 0) {
            Write-Error-Custom "Build failed."
            exit 1
        }
        Write-Success "Build complete. Executable: $buildDir\shi.exe"
    }

    "test" {
        # First build
        if ($ShowVerbose) {
            .\build.ps1 build -BuildType $BuildType -ShowVerbose
        } else {
            .\build.ps1 build -BuildType $BuildType
        }

        # Run tests
        Write-Header "Running tests"
        if (-not (Test-Path "$buildDir\CTestTestfile.cmake")) {
            Write-Error-Custom "Build not configured. Run 'build.ps1 build' first."
            exit 1
        }

        $ctestArgs = @("--output-on-failure", "-C", $BuildType)
        if ($Component) {
            $ctestArgs += "-R"
            $ctestArgs += "^${Component}$"
        }
        Set-Location $buildDir
        ctest @ctestArgs
        $exitCode = $LASTEXITCODE
        Set-Location (Split-Path -Parent $MyInvocation.MyCommand.Path)

        Write-Host ""
        if ($exitCode -eq 0) {
            Write-Success "All tests passed."
        } else {
            Write-Error-Custom "Some tests failed."
        }
        exit $exitCode
    }

    "clean" {
        Write-Header "Cleaning build directory"
        if (Test-Path $buildDir) {
            Remove-Item -Recurse -Force $buildDir
            Write-Success "Build directory removed."
        } else {
            Write-Warning-Custom "Build directory does not exist."
        }
    }

    "distclean" {
        Write-Header "Distclean (build + test artifacts)"
        if (Test-Path $buildDir) {
            Remove-Item -Recurse -Force $buildDir
            Write-Success "Build directory removed."
        }
        # Remove test temp files
        $tmpFiles = Get-ChildItem -Path "tests" -Filter "test_*.tmp" -ErrorAction SilentlyContinue
        if ($tmpFiles) {
            Remove-Item $tmpFiles -Force
            Write-Success "Test artifacts removed."
        } else {
            Write-Warning-Custom "No test artifacts to remove."
        }
    }

    "help" {
        Show-Help
    }
}

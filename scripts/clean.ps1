# ===========================================================================
# clean.ps1 — Clean build artifacts for shi (Huffman compression tool)
#
# Usage:
#   .\clean.ps1              - Remove build directory only
#   .\clean.ps1 -distclean  - Remove build directory and test artifacts
# ===========================================================================

param(
    [switch]$Distclean
)

$ErrorActionPreference = "Stop"

$buildDir = "build"

if ($Distclean) {
    Write-Host "" -ForegroundColor Cyan
    Write-Host "================================================" -ForegroundColor Cyan
    Write-Host "  Distclean (build + test artifacts)" -ForegroundColor Cyan
    Write-Host "================================================" -ForegroundColor Cyan
    Write-Host ""

    if (Test-Path $buildDir) {
        Remove-Item -Recurse -Force $buildDir
        Write-Host "  [OK] Build directory removed." -ForegroundColor Green
    }

    $tmpFiles = Get-ChildItem -Path "tests" -Filter "test_*.tmp" -ErrorAction SilentlyContinue
    if ($tmpFiles) {
        Remove-Item $tmpFiles -Force
        Write-Host "  [OK] Test artifacts removed." -ForegroundColor Green
    } else {
        Write-Host "  [WARN] No test artifacts to remove." -ForegroundColor Yellow
    }
} else {
    Write-Host "" -ForegroundColor Cyan
    Write-Host "================================================" -ForegroundColor Cyan
    Write-Host "  Cleaning build directory" -ForegroundColor Cyan
    Write-Host "================================================" -ForegroundColor Cyan
    Write-Host ""

    if (Test-Path $buildDir) {
        Remove-Item -Recurse -Force $buildDir
        Write-Host "  [OK] Build directory removed." -ForegroundColor Green
    } else {
        Write-Host "  [WARN] Build directory does not exist." -ForegroundColor Yellow
    }
}

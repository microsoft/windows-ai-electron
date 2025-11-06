#!/usr/bin/env pwsh
<#
.SYNOPSIS
    Build script for Windows SDK CLI, npm package, and MSIX bundle
.DESCRIPTION
    This script builds the Windows SDK CLI for both x64 and arm64 architectures,
    creates the npm package, creates MSIX bundle with distribution package, and 
    places all artifacts in an artifacts folder. Run this script from the root of the project.
    
    For CI/CD environments, ensure GitHub CLI is installed and authenticated:
    - Install GitHub CLI: https://cli.github.com/
    - Authenticate: gh auth login
    
.PARAMETER SkipDependencies
    Skip dependency installation (useful for local development)
.EXAMPLE
    .\scripts\build-cli.ps1
.EXAMPLE
    # For local development when dependencies are already installed
    .\scripts\build-cli.ps1 -SkipDependencies
#>

param(
    [switch]$SkipDependencies
)

# Constants
$WinAppCliRepo = "microsoft/WinAppCli"

function Download-WinAppCliTarball {
    param(
        [string]$DestinationPath
    )
    
    # Extract version from package.json
    $PackageJson = Get-Content "package.json" | ConvertFrom-Json
    $WinAppCliDep = $PackageJson.dependencies.'@microsoft/winappcli'
    if ($WinAppCliDep -match "microsoft-winappcli-(.+)\.tgz") {
        $Version = $matches[1]
        Write-Host "[WINAPPCLI] Detected version from package.json: $Version" -ForegroundColor Cyan
    } else {
        Write-Error "Could not detect winappcli version from package.json. Check that @microsoft/winappcli dependency is properly specified."
        exit 1
    }

    $TarballName = "microsoft-winappcli-$Version.tgz"
    $FullDestPath = Join-Path $DestinationPath $TarballName
    
    # Create destination directory if it doesn't exist
    $DestDir = Split-Path $FullDestPath -Parent
    if (-not (Test-Path $DestDir)) {
        New-Item -ItemType Directory -Path $DestDir -Force | Out-Null
    }
    
    # Skip download if file already exists
    if (Test-Path $FullDestPath) {
        Write-Host "[WINAPPCLI] Tarball already exists: $FullDestPath" -ForegroundColor Green
        return
    }

    Write-Host "[WINAPPCLI] Downloading $TarballName from $WinAppCliRepo..." -ForegroundColor Blue

    try {
        # Use GitHub CLI for download (requires gh to be installed and authenticated)
        if (-not (Get-Command "gh" -ErrorAction SilentlyContinue)) {
            Write-Error "GitHub CLI (gh) is required but not found. Please install it: https://cli.github.com/"
            exit 1
        }

        Write-Host "[WINAPPCLI] Using GitHub CLI for download..." -ForegroundColor Gray
        & gh release download "v$Version" --repo $WinAppCliRepo --pattern $TarballName --dir (Split-Path $FullDestPath -Parent)

        if ($LASTEXITCODE -eq 0) {
            Write-Host "[WINAPPCLI] Successfully downloaded via GitHub CLI" -ForegroundColor Green
        } else {
            Write-Error "GitHub CLI download failed. Make sure you're authenticated: gh auth login"
            exit 1
        }
        
    } catch {
        Write-Error "Failed to download winappcli tarball: $($_.Exception.Message)"
        Write-Host "[WINAPPCLI] Make sure you have access to the private repository and are authenticated with GitHub CLI" -ForegroundColor Yellow
        Write-Host "[WINAPPCLI] Run: gh auth login" -ForegroundColor Yellow
        exit 1
    }
}

# Ensure we're running from the project root
$ProjectRoot = $PSScriptRoot | Split-Path -Parent
Write-Host "Project root: $ProjectRoot" -ForegroundColor Gray

Push-Location $ProjectRoot
try
{
    # Define paths
    $ArtifactsPath = "artifacts"

    Write-Host "[*] Starting winapp-windows-ai build process..." -ForegroundColor Green
    Write-Host "Project root: $ProjectRoot" -ForegroundColor Gray

    # Step 0: Setup dependencies
    if (-not $SkipDependencies) {
        Write-Host "[SETUP] Setting up dependencies..." -ForegroundColor Blue
        
        # Download winappcli tarball if needed
        Write-Host "[SETUP] Ensuring winappcli tarball is available..." -ForegroundColor Blue
        Download-WinAppCliTarball -DestinationPath "../winappcli/artifacts"
        
        # Install npm dependencies
        Write-Host "[SETUP] Installing npm dependencies..." -ForegroundColor Blue
        yarn install
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Failed to install npm dependencies"
            exit 1
        }
        
        # Setup winappcli dependency
        Write-Host "[SETUP] Setting up winappcli dependency..." -ForegroundColor Blue
        yarn winapp restore
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Failed to setup winappcli dependency"
            exit 1
        }
    } else {
        Write-Host "[SETUP] Skipping dependency installation (SkipDependencies flag set)" -ForegroundColor Yellow
    }

    Write-Host "[CLEAN] Cleaning artifacts and test results..." -ForegroundColor Yellow
    if (Test-Path $ArtifactsPath) {
        Remove-Item $ArtifactsPath -Recurse -Force
    }

    # Create artifacts directory
    Write-Host "[SETUP] Creating artifacts directory..." -ForegroundColor Blue
    New-Item -ItemType Directory -Path $ArtifactsPath -Force | Out-Null

    # Step 1: Build addon
    Write-Host "[BUILD] Building addon..." -ForegroundColor Blue
    yarn build-winapp-windows-ai
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to build addon"
        exit 1
    }

    # Step 3: Calculate version with build number (moved before publish)
    Write-Host "[VERSION] Calculating package version..." -ForegroundColor Blue

    # Read base version from package.json
    $PackageJsonPath = "$ProjectRoot\package.json"
    if (-not (Test-Path $PackageJsonPath)) {
        Write-Error "package.json not found at $PackageJsonPath"
        exit 1
    }

    $PackageJson = Get-Content $PackageJsonPath | ConvertFrom-Json
    $BaseVersion = $PackageJson.version

    # Get build number
    $BuildNumber = & "$PSScriptRoot\get-build-number.ps1"
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to get build number"
        exit 1
    }

    # Prerelease build: add prerelease number suffix (e.g., "0.1.0-prerelease.73")
    $FullVersion = "$BaseVersion-prerelease.$BuildNumber"
    Write-Host "[VERSION] Using prerelease version (with prerelease suffix)" -ForegroundColor Cyan
    Write-Host "[VERSION] Package version: $FullVersion" -ForegroundColor Cyan

    # Extract semantic version components for assembly versioning
    # BaseVersion should be in format major.minor.patch (e.g., "0.1.0")
    $VersionParts = $BaseVersion -split '\.'
    $MajorVersion = $VersionParts[0]
    $MinorVersion = $VersionParts[1]
    $PatchVersion = $VersionParts[2]

    # Step 6: Prepare npm package
    Write-Host "[NPM] Preparing npm package..." -ForegroundColor Blue

    # Clean npm bin directory first
    npm run clean
    if ($LASTEXITCODE -ne 0) {
        Write-Warning "npm clean failed, continuing..."
    }

    # Backup original package.json
    Write-Host "[NPM] Setting package version to $FullVersion..." -ForegroundColor Blue
    $PackageJsonPath = "package.json"
    Copy-Item $PackageJsonPath "$PackageJsonPath.backup" -Force

    # Update package.json version temporarily
    $PackageJson = Get-Content $PackageJsonPath | ConvertFrom-Json
    $OriginalVersion = $PackageJson.version
    $PackageJson.version = $FullVersion
    $PackageJson | ConvertTo-Json -Depth 100 | Set-Content $PackageJsonPath

    # Step 7: Create npm package tarball
    Write-Host "[PACK] Creating npm package tarball..." -ForegroundColor Blue
    npm pack --pack-destination "$ArtifactsPath"
    $PackResult = $LASTEXITCODE

    # Restore original package.json
    Write-Host "[NPM] Restoring original package.json..." -ForegroundColor Blue
    if (Test-Path "$PackageJsonPath.backup") {
        Move-Item "$PackageJsonPath.backup" $PackageJsonPath -Force
    }

    if ($PackResult -ne 0) {
        Write-Error "Failed to create npm package"
        Pop-Location
        exit 1
    }

    # Build process complete - all artifacts are ready

    # Display results
    Write-Host ""
    Write-Host "[SUCCESS] Build completed successfully!" -ForegroundColor Green
    Write-Host ""
    Write-Host "[VERSION] Package version: $FullVersion" -ForegroundColor Cyan
    Write-Host "[INFO] Artifacts created in: $ArtifactsPath" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Contents:" -ForegroundColor White
    Get-ChildItem $ArtifactsPath | ForEach-Object {
        $size = if ($_.PSIsContainer) { "(folder)" } else { "($([math]::Round($_.Length / 1MB, 2)) MB)" }
        Write-Host "  * $($_.Name) $size" -ForegroundColor Gray
    }

    Write-Host ""
    Write-Host "[DONE] Ready for distribution!" -ForegroundColor Green
}
finally
{
    # Restore original working directory
    Pop-Location
}

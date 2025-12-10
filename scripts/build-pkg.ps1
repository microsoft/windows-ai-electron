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
.PARAMETER Stable
    Build a stable release (uses stable version format without -prerelease suffix)
.EXAMPLE
    .\scripts\build-pkg.ps1
.EXAMPLE
    # For local development when dependencies are already installed
    .\scripts\build-pkg.ps1 -SkipDependencies
.EXAMPLE
    # For stable release
    .\scripts\build-pkg.ps1 -Stable -SkipDependencies
#>

param(
    [switch]$SkipDependencies,
    [switch]$Stable
)


# Ensure we're running from the project root
$ProjectRoot = $PSScriptRoot | Split-Path -Parent
Write-Host "Project root: $ProjectRoot" -ForegroundColor Gray

Push-Location $ProjectRoot
try
{
    # Define paths
    $ArtifactsPath = "artifacts"

    Write-Host "[*] Starting @microsoft/winapp-windows-ai build process..." -ForegroundColor Green
    Write-Host "Project root: $ProjectRoot" -ForegroundColor Gray

    # Step 0: Setup dependencies
    if (-not $SkipDependencies) {
        Write-Host "[SETUP] Setting up dependencies..." -ForegroundColor Blue
        
        # Install npm dependencies (includes @microsoft/winappcli from npm)
        Write-Host "[SETUP] Installing npm dependencies..." -ForegroundColor Blue
        npm install
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Failed to install npm dependencies"
            exit 1
        }
        
        # Setup winappcli dependency
        Write-Host "[SETUP] Setting up winappcli dependency..." -ForegroundColor Blue
        npx winapp restore
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
    npm run build-all
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

    if ($Stable) {
        # Stable build: use get-stable-version.ps1 to calculate next stable version
        $FullVersion = & "$PSScriptRoot\get-stable-version.ps1"
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Failed to get stable version"
            exit 1
        }
        Write-Host "[VERSION] Using stable version (no prerelease suffix)" -ForegroundColor Cyan
        Write-Host "[VERSION] Package version: $FullVersion" -ForegroundColor Cyan
    } else {
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
    }

    # Prepare git tag (will be created at the end if build succeeds)
    $GitTag = "v$FullVersion"
    Write-Host "[VERSION] Will create git tag: $GitTag (after successful build)" -ForegroundColor Cyan

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

    # Build process complete - now create the git tag
    Write-Host "[GIT] Creating git tag: $GitTag" -ForegroundColor Blue
    try {
        # Check if tag already exists
        $existingTag = git tag -l $GitTag 2>$null
        if ([string]::IsNullOrEmpty($existingTag)) {
            git tag $GitTag
            if ($LASTEXITCODE -eq 0) {
                Write-Host "[GIT] Successfully created tag: $GitTag" -ForegroundColor Green
            } else {
                Write-Warning "[GIT] Failed to create git tag. Build succeeded but tag was not created."
            }
        } else {
            Write-Host "[GIT] Tag $GitTag already exists" -ForegroundColor Yellow
        }
    } catch {
        Write-Warning "[GIT] Failed to create git tag: $($_.Exception.Message). Build succeeded but tag was not created."
    }

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

#!/usr/bin/env pwsh
<#
.SYNOPSIS
    Calculates the next stable release version based on existing stable release tags.

.DESCRIPTION
    This script finds all stable release tags matching the base version pattern (e.g., v0.1.*),
    determines the highest patch number, and returns the next version number.
    
    For version 0.1 in package.json:
    - If no v0.1.* tags exist → returns 0.1.0
    - If v0.1.0 exists → returns 0.1.1
    - If v0.1.5 is highest → returns 0.1.6

.PARAMETER VersionFile
    Path to the package.json file. Defaults to ../package.json relative to this script.

.OUTPUTS
    Returns the full version string (e.g., "0.1.0")

.EXAMPLE
    .\get-stable-version.ps1
    # For version 0.1 with existing tag v0.1.2
    # Returns: 0.1.3
#>

param(
    [string]$VersionFile = "$PSScriptRoot\..\package.json"
)

# Ensure we're in a git repository
if (-not (Test-Path .git)) {
    Write-Error "Not in a git repository root. Please run from repository root or ensure .git exists."
    exit 1
}

# Ensure package.json exists
if (-not (Test-Path $VersionFile)) {
    Write-Error "package.json not found at: $VersionFile"
    exit 1
}

try {
    # Read the current base version from package.json (should be in format major.minor, e.g., "0.1")
    $packageJson = Get-Content $VersionFile | ConvertFrom-Json
    $baseVersion = $packageJson.version
    
    Write-Host "[STABLE] Base version from package.json: $baseVersion" -ForegroundColor Cyan
    
    # Validate format (should be major.minor)
    if ($baseVersion -notmatch '^\d+\.\d+$') {
        Write-Error "Base version must be in format major.minor (e.g., '0.1'), got: $baseVersion"
        exit 1
    }
    
    # Get all git tags that match the stable release pattern (v{major}.{minor}.*)
    $tagPattern = "v$baseVersion.*"
    $allTags = git tag -l 2>$null
    
    if ([string]::IsNullOrEmpty($allTags)) {
        Write-Host "[STABLE] No git tags found, starting at $baseVersion.0" -ForegroundColor Yellow
        Write-Output "$baseVersion.0"
        exit 0
    }
    
    # Filter tags that match our stable version pattern (exclude prerelease tags)
    # Pattern: v{major}.{minor}.{patch} (no -prerelease suffix)
    $matchingTags = $allTags | Where-Object { 
        $_ -match "^v$baseVersion\.\d+$" 
    }
    
    $patchNumbers = @()
    
    foreach ($tag in $matchingTags) {
        if ($tag -match "^v$baseVersion\.(\d+)$") {
            $patchNumbers += [int]$matches[1]
            Write-Host "[STABLE] Found existing stable tag: $tag (patch $($matches[1]))" -ForegroundColor Gray
        }
    }
    
    if ($patchNumbers.Count -eq 0) {
        Write-Host "[STABLE] No stable release tags found for version $baseVersion, starting at $baseVersion.0" -ForegroundColor Yellow
        Write-Output "$baseVersion.0"
    } else {
        $highestPatch = ($patchNumbers | Measure-Object -Maximum).Maximum
        $nextPatch = $highestPatch + 1
        $nextVersion = "$baseVersion.$nextPatch"
        Write-Host "[STABLE] Highest existing stable release: $baseVersion.$highestPatch, next: $nextVersion" -ForegroundColor Green
        Write-Output $nextVersion
    }
    
} catch {
    Write-Error "Failed to calculate stable version number: $_"
    exit 1
}

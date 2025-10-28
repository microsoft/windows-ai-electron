<#
.SYNOPSIS
    Calculates the build number based on commits since version.json was last changed.

.DESCRIPTION
    This script counts the number of commits since version.json was last modified.
    The build number starts at 1 when version.json is changed, and increments with each subsequent commit.

.PARAMETER VersionFile
    Path to the version.json file. Defaults to ../version.json relative to this script.

.OUTPUTS
    Returns the build number as an integer.

.EXAMPLE
    .\get-build-number.ps1
    # Returns: 25 (if there have been 25 commits since version.json changed)
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
    # Get the commit hash where package.json was last changed
    $lastVersionCommit = git log -1 --format="%H" -- $VersionFile 2>$null
    
    if ([string]::IsNullOrEmpty($lastVersionCommit)) {
        # package.json hasn't been committed yet, count all commits
        $buildNumber = git rev-list --count HEAD 2>$null
        if ([string]::IsNullOrEmpty($buildNumber)) {
            # No commits yet
            Write-Output "1"
            exit 0
        }
    } else {
        # Count commits since version.json was last changed (exclusive of that commit)
        $buildNumber = git rev-list --count "$lastVersionCommit..HEAD" 2>$null
        
        if ([string]::IsNullOrEmpty($buildNumber)) {
            $buildNumber = 0
        }
        
        # Add 1 because we want build numbers to start at 1, not 0
        $buildNumber = [int]$buildNumber + 1
    }
    
    Write-Output $buildNumber
} catch {
    Write-Error "Failed to calculate build number: $_"
    exit 1
}
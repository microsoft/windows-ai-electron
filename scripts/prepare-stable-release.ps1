#!/usr/bin/env pwsh
<#
.SYNOPSIS
    Updates package.json version and creates/pushes a stable release branch.

.DESCRIPTION
    This script helps prepare for a stable release by:
    1. Updating package.json to the specified major.minor version
    2. Creating a new stable branch (e.g., v0.1-stable)
    3. Optionally pushing the branch to trigger the ADO pipeline

.PARAMETER Version
    The major.minor version to set (e.g., "0.1", "0.2", "1.0")

.PARAMETER Push
    If specified, pushes the stable branch to origin to trigger the pipeline

.EXAMPLE
    .\prepare-stable-release.ps1 -Version "0.1"
    # Updates package.json to 0.1 and creates v0.1-stable branch locally

.EXAMPLE
    .\prepare-stable-release.ps1 -Version "0.2" -Push
    # Updates package.json to 0.2, creates v0.2-stable branch, and pushes to trigger pipeline
#>

param(
    [Parameter(Mandatory=$true)]
    [string]$Version,
    
    [switch]$Push
)

$ProjectRoot = $PSScriptRoot | Split-Path -Parent
$PackageJsonPath = "$ProjectRoot\package.json"

# Validate version format
if ($Version -notmatch '^\d+\.\d+$') {
    Write-Error "Version must be in major.minor format (e.g., '0.1', '0.2', '1.0')"
    exit 1
}

# Ensure package.json exists
if (-not (Test-Path $PackageJsonPath)) {
    Write-Error "package.json not found at: $PackageJsonPath"
    exit 1
}

try {
    Push-Location $ProjectRoot
    
    # Check for uncommitted changes
    $gitStatus = git status --porcelain
    if ($gitStatus) {
        Write-Warning "You have uncommitted changes. Please commit or stash them first."
        Write-Host "`nUncommitted changes:" -ForegroundColor Yellow
        Write-Host $gitStatus
        Pop-Location
        exit 1
    }
    
    # Read current package.json
    Write-Host "[INFO] Reading package.json..." -ForegroundColor Cyan
    $packageJson = Get-Content $PackageJsonPath | ConvertFrom-Json
    $currentVersion = $packageJson.version
    
    Write-Host "[INFO] Current version: $currentVersion" -ForegroundColor Gray
    Write-Host "[INFO] New version: $Version" -ForegroundColor Green
    
    # Update version
    $packageJson.version = $Version
    $packageJson | ConvertTo-Json -Depth 100 | Set-Content $PackageJsonPath
    
    Write-Host "[SUCCESS] Updated package.json version to $Version" -ForegroundColor Green
    
    # Create stable branch name
    $branchName = "v$Version-stable"
    
    # Check if branch already exists
    $branchExists = git rev-parse --verify $branchName 2>$null
    if ($LASTEXITCODE -eq 0) {
        Write-Host "[INFO] Branch '$branchName' already exists" -ForegroundColor Yellow
        $response = Read-Host "Do you want to switch to it? (y/n)"
        if ($response -eq 'y') {
            git checkout $branchName
            Write-Host "[SUCCESS] Switched to existing branch '$branchName'" -ForegroundColor Green
        }
    } else {
        # Create new branch
        Write-Host "[INFO] Creating new branch: $branchName" -ForegroundColor Cyan
        git checkout -b $branchName
        
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Failed to create branch"
            Pop-Location
            exit 1
        }
        
        Write-Host "[SUCCESS] Created and switched to branch '$branchName'" -ForegroundColor Green
    }
    
    # Commit the version change
    git add package.json
    git commit -m "chore: bump version to $Version for stable release"
    
    if ($LASTEXITCODE -ne 0) {
        Write-Warning "Failed to commit changes (may already be committed)"
    } else {
        Write-Host "[SUCCESS] Committed version change" -ForegroundColor Green
    }
    
    # Push if requested
    if ($Push) {
        Write-Host "[INFO] Pushing branch to origin..." -ForegroundColor Cyan
        git push origin $branchName
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "[SUCCESS] Pushed branch '$branchName' to origin" -ForegroundColor Green
            Write-Host ""
            Write-Host "🚀 The ADO pipeline will now:" -ForegroundColor Cyan
            Write-Host "   1. Calculate the next stable version (e.g., $Version.0, $Version.1, etc.)" -ForegroundColor Gray
            Write-Host "   2. Build the npm package" -ForegroundColor Gray
            Write-Host "   3. Create a GitHub release with tag v$Version.X" -ForegroundColor Gray
            Write-Host "   4. Publish to NPM via ESRP" -ForegroundColor Gray
            Write-Host ""
            Write-Host "Monitor the pipeline at: https://dev.azure.com/microsoft/pde-oss/_build" -ForegroundColor Yellow
        } else {
            Write-Error "Failed to push branch"
            Pop-Location
            exit 1
        }
    } else {
        Write-Host ""
        Write-Host "[NEXT STEPS]" -ForegroundColor Yellow
        Write-Host "To trigger the stable release pipeline, push the branch:" -ForegroundColor Gray
        Write-Host "  git push origin $branchName" -ForegroundColor Cyan
        Write-Host ""
        Write-Host "Or run this script again with -Push flag:" -ForegroundColor Gray
        Write-Host "  .\scripts\prepare-stable-release.ps1 -Version $Version -Push" -ForegroundColor Cyan
    }
    
    Pop-Location
    
} catch {
    Write-Error "Failed to prepare stable release: $_"
    Pop-Location
    exit 1
}

# Release Process

This document describes the release process for `@microsoft/winapp-windows-ai`, including both prerelease and stable releases.

## Overview

The project uses two separate release pipelines:
- **Prerelease Pipeline** (GitHub Actions) - Automated builds for every push to `main`
- **Stable Release Pipeline** (Azure DevOps) - Production releases from stable branches

## Prerelease Releases

### Pipeline
- **Platform**: GitHub Actions
- **File**: `.github/workflows/build-package.yml`
- **Trigger**: Push to `main` branch
- **Release Type**: Pre-release

### Versioning
- `package.json` version format: `major.minor.patch` (e.g., `"0.1.5"`)
- Generated version: `{version}-prerelease.{buildNumber}` (e.g., `0.1.5-prerelease.3`)
- Git tag: `v{version}-prerelease.{buildNumber}` (e.g., `v0.1.5-prerelease.3`)

### Process
1. Developer pushes code to `main` branch
2. Pipeline calculates next prerelease number based on existing tags
3. Builds native addon for x64 and ARM64
4. Creates npm package tarball
5. Creates GitHub pre-release with tag
6. Uploads tarball as release asset

### Installation
Download the prerelease tarball from GitHub Releases:
```bash
npm install microsoft-winapp-windows-ai-{version}-prerelease.{buildNumber}.tgz
```

## Stable Releases

### Pipeline
- **Platform**: Azure DevOps
- **File**: `.pipelines/stable-release.yml`
- **Project**: `microsoft/pde-oss`
- **Trigger**: Push to branches matching `v0.*-stable` pattern
- **Release Type**: Stable

### Versioning
- `package.json` version format: `major.minor` (e.g., `"0.1"`)
- Generated version: `{version}.{patch}` (e.g., `0.1.0`, `0.1.1`)
- Git tag: `v{version}.{patch}` (e.g., `v0.1.0`, `v0.1.1`)

### Version Calculation
The pipeline automatically determines the next patch version:
- If no `v0.1.*` tags exist → creates `v0.1.0`
- If `v0.1.0` exists → creates `v0.1.1`
- If `v0.1.5` is highest → creates `v0.1.6`

### Process Stages
1. **Build Stage**
   - Calculate next version from existing tags
   - Build native addon
   - Create npm package
   - Upload artifacts

2. **GitHub Release Stage**
   - Create stable GitHub release
   - Tag: `v{version}`
   - Upload tarball
   - Generate release notes

3. **NPM Publish Stage** (if ESRP enabled)
   - Publish to NPM registry via ESRP
   - Requires approval from configured approvers
   - Production-ready release

## Creating Your First Stable Release

### Prerequisites
- Access to `microsoft/pde-oss` Azure DevOps project
- GitHub service connection configured in ADO
- Signing secrets configured (reused from WinAppCli)

### Steps

1. **Update package.json** to major.minor format:
   ```json
   {
     "version": "0.1"
   }
   ```

2. **Use the helper script**:
   ```powershell
   .\scripts\prepare-stable-release.ps1 -Version "0.1" -Push
   ```
   
   This will:
   - Update `package.json` version
   - Create `v0.1-stable` branch
   - Commit the change
   - Push to trigger the pipeline

3. **Monitor the pipeline**:
   - Go to https://dev.azure.com/microsoft/pde-oss/_build
   - Watch the build, release, and publish stages

4. **Verify the release**:
   - Check GitHub releases for new `v0.1.0` release
   - Verify npm package is published: `npm view @microsoft/winapp-windows-ai`

## Creating Patch Releases

To release a bug fix or patch (e.g., `v0.1.1` after `v0.1.0`):

1. **Checkout the stable branch**:
   ```bash
   git checkout v0.1-stable
   ```

2. **Cherry-pick or apply fixes**:
   ```bash
   git cherry-pick <commit-hash>
   # or make changes directly
   git commit -am "Fix: description"
   ```

3. **Push to trigger pipeline**:
   ```bash
   git push origin v0.1-stable
   ```

4. **Pipeline automatically**:
   - Detects `v0.1.0` exists
   - Builds version `v0.1.1`
   - Creates release and publishes

## Creating New Minor Versions

To create a new minor version (e.g., `0.2.0`):

1. **Update package.json**:
   ```json
   {
     "version": "0.2"
   }
   ```

2. **Create new stable branch**:
   ```powershell
   .\scripts\prepare-stable-release.ps1 -Version "0.2" -Push
   ```

3. **Pipeline creates** `v0.2.0` release

## Comparison Table

| Feature | Prerelease | Stable |
|---------|-----------|--------|
| **Trigger** | Push to `main` | Push to `v0.*-stable` |
| **Platform** | GitHub Actions | Azure DevOps |
| **Version** | `0.1.5-prerelease.3` | `0.1.0` |
| **package.json** | `"0.1.5"` | `"0.1"` |
| **Tag** | `v0.1.5-prerelease.3` | `v0.1.0` |
| **Release Type** | Pre-release | Stable |
| **NPM Publish** | No | Yes (via ESRP) |
| **Distribution** | GitHub only | GitHub + NPM |
| **Audience** | Testing, CI/CD | Production |

## Version Examples

### Prerelease Examples
| package.json | Existing Tags | Next Version |
|--------------|---------------|--------------|
| `"0.1.5"` | None | `0.1.5-prerelease.1` |
| `"0.1.5"` | `v0.1.5-prerelease.2` | `0.1.5-prerelease.3` |

### Stable Examples
| package.json | Existing Tags | Next Version |
|--------------|---------------|--------------|
| `"0.1"` | None | `0.1.0` |
| `"0.1"` | `v0.1.0` | `0.1.1` |
| `"0.1"` | `v0.1.0`, `v0.1.1`, `v0.1.2` | `0.1.3` |
| `"0.2"` | None | `0.2.0` |

## Helper Scripts

### `scripts/get-build-number.ps1`
Calculates the next prerelease build number for the current version.
```powershell
.\scripts\get-build-number.ps1
# Returns: 3 (for version 0.1.5-prerelease.3)
```

### `scripts/get-stable-version.ps1`
Calculates the next stable version for the current base version.
```powershell
.\scripts\get-stable-version.ps1
# Returns: 0.1.2 (if 0.1.1 exists)
```

### `scripts/prepare-stable-release.ps1`
Prepares and pushes a stable release branch.
```powershell
# Create and push locally
.\scripts\prepare-stable-release.ps1 -Version "0.1"

# Create and push to trigger pipeline
.\scripts\prepare-stable-release.ps1 -Version "0.1" -Push
```

## Troubleshooting

### Prerelease Issues

**Pipeline creates wrong version**:
- Ensure git tags use `v` prefix (`v0.1.5-prerelease.1`)
- Check `get-build-number.ps1` is finding existing tags
- Verify fetch depth is 0 in workflow

**Build fails**:
- Check Node.js and .NET versions match requirements
- Verify dependencies installed correctly
- Review build logs for specific errors

### Stable Release Issues

**Pipeline not triggering**:
- Verify branch name matches `v0.*-stable` pattern
- Check pipeline is enabled in ADO
- Confirm you have push permissions

**Version calculation fails**:
- Ensure `package.json` version is `major.minor` format
- Check git fetch depth is 0
- Verify no tag conflicts

**ESRP publish fails**:
- Verify all signing variables are configured
- Check service connection is valid
- Confirm approver emails are set
- Review ESRP logs for specific errors

**GitHub release fails**:
- Verify GitHub service connection
- Check PAT has `repo` scope
- Ensure tag doesn't already exist

## ADO Setup for Stable Releases

For detailed setup instructions for the Azure DevOps stable release pipeline, see [`.pipelines/README.md`](.pipelines/README.md).

Quick checklist:
- [ ] Access to `microsoft/pde-oss` project
- [ ] GitHub service connection: `GitHub-winapp-windows-ai`
- [ ] Signing variable group linked
- [ ] Pipeline imported and configured

## Related Files

### Workflows
- `.github/workflows/build-package.yml` - Prerelease pipeline (GitHub Actions)
- `.pipelines/stable-release.yml` - Stable release pipeline (Azure DevOps)

### Scripts
- `scripts/build-pkg.ps1` - Main build script (used by both pipelines)
- `scripts/get-build-number.ps1` - Prerelease version calculator
- `scripts/get-stable-version.ps1` - Stable version calculator
- `scripts/prepare-stable-release.ps1` - Stable release helper

### Documentation
- `.pipelines/README.md` - ADO pipeline setup guide
- `.pipelines/QUICKSTART.md` - Quick reference for stable releases

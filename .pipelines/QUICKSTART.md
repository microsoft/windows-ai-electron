# Quick Reference: Stable Release Pipeline

## 📁 Files Created
- `.pipelines/stable-release.yml` - ADO pipeline
- `.pipelines/README.md` - Full setup guide
- `.pipelines/SUMMARY.md` - Overview & examples
- `scripts/get-stable-version.ps1` - Version calculator
- `scripts/prepare-stable-release.ps1` - Release helper

## 🎯 Quick Start

### Create Your First Stable Release
```powershell
# Update to version 0.1 and push stable branch
.\scripts\prepare-stable-release.ps1 -Version "0.1" -Push
```

### Create a Patch Release
```powershell
# Fix bugs on existing stable branch
git checkout v0.1-stable
git commit -am "Fix: bug description"
git push origin v0.1-stable
```

### Create New Minor Version
```powershell
# Bump to 0.2
.\scripts\prepare-stable-release.ps1 -Version "0.2" -Push
```

## 🔧 ADO Setup Checklist

### ✅ Permissions Needed
- [ ] Access to `microsoft/pde-oss` project
- [ ] Build/Release pipeline permissions
- [ ] Access to signing variable group (reused from WinAppCli)

### ✅ Service Connection
- [ ] Create GitHub service connection: `GitHub-winapp-windows-ai`
- [ ] Use PAT with `repo` scope
- [ ] Test connection works

### ✅ Pipeline Configuration
- [ ] Import `.pipelines/stable-release.yml` in ADO
- [ ] Link signing variable group
- [ ] Configure branch filter: `v0.*-stable`
- [ ] Save pipeline (don't run yet)

### ✅ Variables (Already Exist - Just Link Them)
- [ ] `SigningServiceName`
- [ ] `SigningAppId`
- [ ] `SigningTenantId`
- [ ] `SigningAKVName`
- [ ] `SigningAuthCertName`
- [ ] `SigningSignCertName`
- [ ] `EsrpOwnersEmail`
- [ ] `EsrpApproversEmail`

## 📊 Version Examples

| package.json | Existing Tags | Next Version |
|--------------|---------------|--------------|
| `"0.1"` | None | `0.1.0` |
| `"0.1"` | `v0.1.0` | `0.1.1` |
| `"0.1"` | `v0.1.0`, `v0.1.1`, `v0.1.2` | `0.1.3` |
| `"0.2"` | None | `0.2.0` |
| `"1.0"` | None | `1.0.0` |

## 🚀 Pipeline Stages

1. **Build**
   - Calculate version from tags
   - Build native addon
   - Create npm package
   - Upload artifacts

2. **Release_GitHub**
   - Create GitHub release
   - Tag: `v{version}`
   - Upload tarball
   - Generate release notes

3. **Release_Npm** (if DoEsrp=true)
   - Publish to NPM via ESRP
   - Requires approval
   - Production release

## 🔍 Troubleshooting

### Pipeline doesn't trigger
```powershell
# Verify branch name format
git branch --show-current
# Should be: v0.1-stable, v0.2-stable, etc.
```

### Version calculation fails
```powershell
# Test version script locally
.\scripts\get-stable-version.ps1
```

### GitHub release fails
- Check service connection: `GitHub-winapp-windows-ai`
- Verify PAT has `repo` scope
- Ensure tag doesn't already exist

### ESRP publish fails
- Verify variable group is linked
- Check all signing secrets are set
- Confirm EsrpOwnersEmail is valid

## 📚 Full Documentation

See `.pipelines/README.md` for:
- Complete setup instructions
- Detailed troubleshooting
- Permission requirements
- Pipeline architecture

## 🔗 Related Pipelines

- **Prerelease**: `.github/workflows/build-package.yml` (GitHub Actions)
- **Reference**: WinAppCli pipeline at `microsoft/WinAppCli/.pipelines/release.yml`

## 💡 Pro Tips

1. **Test locally first**: Run `get-stable-version.ps1` to verify version calculation
2. **Use helper script**: `prepare-stable-release.ps1` handles all setup steps
3. **Check existing tags**: `git tag -l "v0.1.*"` before creating release
4. **Monitor pipeline**: https://dev.azure.com/microsoft/pde-oss/_build
5. **Reuse secrets**: All signing variables already exist from WinAppCli

## 🆘 Need Help?

- **Setup questions**: See `.pipelines/README.md`
- **Version questions**: See `.pipelines/SUMMARY.md`
- **ADO access**: Contact pde-oss admins
- **ESRP issues**: Reference WinAppCli configuration

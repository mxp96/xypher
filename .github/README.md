# GitHub Actions Workflows

## Automated Workflows

### Test Workflow (`test.yml`)

Runs automatically on:
- Every push to `master` or `develop` branch
- Every pull request to `master`
- Weekly schedule (Sunday 00:00 UTC)

Tests on: Linux, macOS, Windows

### Manual Workflows

#### Build Workflow (`build.yml`)

Manually build for specific platforms:
1. Go to Actions tab
2. Select "Build" workflow
3. Click "Run workflow"
4. Choose target: `linux`, `windows`, `macos`, or `all`
5. Download artifacts

#### Release Workflow (`release.yml`)

Create release with binaries:
1. Go to Actions tab
2. Select "Release" workflow
3. Click "Run workflow"
4. Enter:
   - Version: `v1.0.0`
   - Title: `Xypher v1.0.0 - Initial Release`
   - Notes: Release description
5. Creates GitHub release with binaries for all platforms

## Artifacts

Build artifacts are uploaded and available for download for 90 days.

## Issue Templates

- Bug Report: Standard bug reporting template
- Feature Request: Request new features

## PR Template

Includes checklist for code quality and testing.


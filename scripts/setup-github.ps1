# GitHub Repository Setup Script
# Run after: gh auth login

$ErrorActionPreference = "Stop"
$GH = "C:\Program Files\GitHub CLI\gh.exe"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "GitHub Repository Setup" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check auth
Write-Host "Checking authentication..." -ForegroundColor Yellow
& $GH auth status
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error: Not authenticated. Run: gh auth login" -ForegroundColor Red
    exit 1
}

# Repository details
$OWNER = "owner"
$REPO = "eda-app"
$DESCRIPTION = "EDA Desktop Application - Cross-platform Qt5 C++ application"

# Create repository
Write-Host ""
Write-Host "Creating repository: $OWNER/$REPO..." -ForegroundColor Green
& $GH repo create "$REPO" --public --description "$DESCRIPTION" --source=. --remote=origin --push
if ($LASTEXITCODE -ne 0) {
    Write-Host "Repository may already exist, continuing..." -ForegroundColor Yellow
}

# Create initial files
Write-Host ""
Write-Host "Adding files to git..." -ForegroundColor Green
git init
git add .
git commit -m "Initial commit: EDA project structure"

# Push to GitHub
Write-Host ""
Write-Host "Pushing to GitHub..." -ForegroundColor Green
git branch -M main
git remote add origin "https://github.com/$OWNER/$REPO.git" 2>$null
git push -u origin main

# Create dev branch
Write-Host ""
Write-Host "Creating dev branch..." -ForegroundColor Green
git checkout -b dev
git push -u origin dev
git checkout main

# Setup branch protection (requires admin)
Write-Host ""
Write-Host "Configuring branch protection..." -ForegroundColor Yellow
Write-Host "Note: Branch protection requires manual setup in GitHub UI" -ForegroundColor Gray
Write-Host "URL: https://github.com/$OWNER/$REPO/settings/branches" -ForegroundColor Cyan

# Add team members (placeholder)
Write-Host ""
Write-Host "Adding team members..." -ForegroundColor Yellow
Write-Host "Run manually:" -ForegroundColor Gray
Write-Host "  gh api repos/$OWNER/$REPO/collaborators -X PUT -f username=USER1 -f permission=push" -ForegroundColor Gray
Write-Host "  gh api repos/$OWNER/$REPO/collaborators -X PUT -f username=USER2 -f permission=push" -ForegroundColor Gray

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Setup Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Repository URL: https://github.com/$OWNER/$REPO" -ForegroundColor Cyan
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "1. Configure branch protection rules in GitHub UI" -ForegroundColor Gray
Write-Host "2. Add team members as collaborators" -ForegroundColor Gray
Write-Host "3. Enable GitHub Actions in repository settings" -ForegroundColor Gray
Write-Host ""

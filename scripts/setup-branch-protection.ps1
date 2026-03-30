# GitHub Branch Protection Setup
# Run after: gh auth login and repo creation

$ErrorActionPreference = "Stop"
$GH = "C:\Program Files\GitHub CLI\gh.exe"

$OWNER = "owner"
$REPO = "eda-app"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Branch Protection Setup" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check auth
Write-Host "Checking authentication..." -ForegroundColor Yellow
& $GH auth status
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error: Not authenticated. Run: gh auth login" -ForegroundColor Red
    exit 1
}

# Configure main branch protection
Write-Host ""
Write-Host "Configuring 'main' branch protection..." -ForegroundColor Green

$MAIN_RULE = @"
{
  "required_status_checks": {
    "strict": true,
    "contexts": ["CI (windows-latest, 5.15.2, Debug)", "CI (windows-latest, 5.15.2, Release)", "CI (ubuntu-20.04, 5.15.2, Debug)", "CI (ubuntu-20.04, 5.15.2, Release)"]
  },
  "required_pull_request_reviews": {
    "required_approving_review_count": 1,
    "dismiss_stale_reviews": true,
    "require_code_owner_reviews": false
  },
  "enforce_admins": true,
  "required_linear_history": false,
  "allow_force_pushes": false,
  "allow_deletions": false,
  "block_creations": false,
  "required_conversation_resolution": true
}
"@

Write-Host "API Call: PUT /repos/$OWNER/$REPO/branches/main/protection" -ForegroundColor Gray
Write-Host "Body: $MAIN_RULE" -ForegroundColor Gray

# Uncomment to execute:
# & $GH api repos/$OWNER/$REPO/branches/main/protection -X PUT -F required_status_checks='{"strict":true,"contexts":["CI"]}' -F required_pull_request_reviews='{"required_approving_review_count":1}'

Write-Host ""
Write-Host "Note: Using GitHub UI is recommended for branch protection setup" -ForegroundColor Yellow
Write-Host "URL: https://github.com/$OWNER/$REPO/settings/branches/main" -ForegroundColor Cyan

# Configure dev branch (less restrictive)
Write-Host ""
Write-Host "Configuring 'dev' branch..." -ForegroundColor Green
Write-Host "Dev branch: Direct pushes allowed, CI required" -ForegroundColor Gray

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Setup Script Complete" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Manual steps required:" -ForegroundColor Yellow
Write-Host "1. Go to: https://github.com/$OWNER/$REPO/settings/branches" -ForegroundColor Cyan
Write-Host "2. Add branch protection rule for 'main':" -ForegroundColor Gray
Write-Host "   - Require pull request reviews before merging (1 reviewer)" -ForegroundColor Gray
Write-Host "   - Require status checks to pass before merging" -ForegroundColor Gray
Write-Host "   - Include: CI (all matrix combinations)" -ForegroundColor Gray
Write-Host "   - Don't allow bypassing the above requirements" -ForegroundColor Gray
Write-Host ""
Write-Host "3. For 'dev' branch: No protection (allow direct pushes)" -ForegroundColor Gray
Write-Host ""

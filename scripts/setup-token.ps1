# GitHub Token 配置脚本
# 用于团队统一认证

$GH = "C:\Program Files\GitHub CLI\gh.exe"
$REPO = "https://github.com/redfalsh1/eda-app.git"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "GitHub Token Configuration" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Get token from gh CLI
Write-Host "Getting token from GitHub CLI..." -ForegroundColor Yellow
$TOKEN = & $GH auth token

if ([string]::IsNullOrEmpty($TOKEN)) {
    Write-Host "Error: Failed to get token. Please run: gh auth login" -ForegroundColor Red
    exit 1
}

Write-Host "✓ Token retrieved successfully" -ForegroundColor Green
Write-Host ""

# Set environment variable (current session)
Write-Host "Setting GH_TOKEN environment variable (current session)..." -ForegroundColor Yellow
$env:GH_TOKEN = $TOKEN
Write-Host "✓ GH_TOKEN set for current session" -ForegroundColor Green
Write-Host ""

# Configure git remote to use HTTPS
Write-Host "Configuring git remote to use HTTPS..." -ForegroundColor Yellow
git remote set-url origin $REPO 2>$null
if ($LASTEXITCODE -ne 0) {
    git remote add origin $REPO
}
Write-Host "✓ Git remote configured: $REPO" -ForegroundColor Green
Write-Host ""

# Configure git credential helper
Write-Host "Configuring git credential helper..." -ForegroundColor Yellow
git config --global credential.helper wincred
Write-Host "✓ Credential helper configured" -ForegroundColor Green
Write-Host ""

# Test authentication
Write-Host "Testing authentication..." -ForegroundColor Yellow
$testResult = & $GH auth status 2>&1
if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ Authentication successful" -ForegroundColor Green
} else {
    Write-Host "⚠ Authentication test failed, but token is set" -ForegroundColor Yellow
}
Write-Host ""

# Display configuration
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Configuration Summary" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Repository: $REPO" -ForegroundColor Cyan
Write-Host "Auth Method: Token (HTTPS)" -ForegroundColor Cyan
Write-Host "GH_TOKEN: Set (current session)" -ForegroundColor Cyan
Write-Host ""
Write-Host "To make GH_TOKEN permanent, run:" -ForegroundColor Yellow
Write-Host '[Environment]::SetEnvironmentVariable("GH_TOKEN", "$TOKEN", "User")' -ForegroundColor Gray
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Setup Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "1. git pull origin dev" -ForegroundColor Gray
Write-Host "2. git push origin dev" -ForegroundColor Gray
Write-Host ""

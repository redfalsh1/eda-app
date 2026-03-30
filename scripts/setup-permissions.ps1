# GitHub 团队权限配置脚本
# 运行前确保已认证：gh auth login

$GH = "C:\Program Files\GitHub CLI\gh.exe"
$OWNER = "redfalsh1"
$REPO = "eda-app"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "GitHub Team Permissions Setup" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check auth
Write-Host "Checking authentication..." -ForegroundColor Yellow
& $GH auth status
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error: Not authenticated. Run: gh auth login" -ForegroundColor Red
    exit 1
}

# Team members
$TEAM = @(
    @{Username="eda_cpp_dev"; Permission="push"; Role="C++ Developer"},
    @{Username="eda_qt_dev"; Permission="push"; Role="Qt Developer"},
    @{Username="eda_qa"; Permission="push"; Role="QA Engineer"},
    @{Username="eda_devops"; Permission="push"; Role="DevOps Engineer"}
)

Write-Host "Repository: https://github.com/$OWNER/$REPO" -ForegroundColor Cyan
Write-Host ""
Write-Host "Adding team members..." -ForegroundColor Green
Write-Host ""

foreach ($member in $TEAM) {
    $username = $member.Username
    $permission = $member.Permission
    
    Write-Host "Adding: $username ($permission)" -ForegroundColor Yellow
    
    # Using GitHub CLI to add collaborator
    # Note: This requires the user to have admin access
    $cmd = "& `$GH api repos/`$OWNER/`$REPO/collaborators/$username -X PUT -F permission=`$permission"
    Write-Host "  Command: $cmd" -ForegroundColor Gray
    
    # Uncomment to execute:
    # & $GH api repos/$OWNER/$REPO/collaborators/$username -X PUT -F permission=$permission
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Manual Setup (Recommended)" -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "URL: https://github.com/$OWNER/$REPO/settings/access" -ForegroundColor Cyan
Write-Host ""
Write-Host "Steps:" -ForegroundColor Gray
Write-Host "1. Click 'Add people'" -ForegroundColor Gray
Write-Host "2. Search for GitHub username" -ForegroundColor Gray
Write-Host "3. Select permission: Write" -ForegroundColor Gray
Write-Host "4. Click 'Add [username] to this repository'" -ForegroundColor Gray
Write-Host ""
Write-Host "Recommended permissions:" -ForegroundColor Yellow
Write-Host "  - TechLead: Admin" -ForegroundColor Gray
Write-Host "  - Developers: Write" -ForegroundColor Gray
Write-Host "  - QA: Write" -ForegroundColor Gray
Write-Host ""

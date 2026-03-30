# GitHub Team Members Setup
# Run after: gh auth login and repo creation
# Update TEAM_MEMBERS with actual GitHub usernames

$ErrorActionPreference = "Stop"
$GH = "C:\Program Files\GitHub CLI\gh.exe"

$OWNER = "owner"
$REPO = "eda-app"

# Team members - Update with actual GitHub usernames
$TEAM_MEMBERS = @(
    @{Username="user1"; Permission="push"; Role="Developer"},
    @{Username="user2"; Permission="push"; Role="Developer"},
    @{Username="user3"; Permission="push"; Role="Developer"},
    @{Username="user4"; Permission="push"; Role="Developer"},
    @{Username="user5"; Permission="push"; Role="Developer"}
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "GitHub Team Members Setup" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Repository: https://github.com/$OWNER/$REPO" -ForegroundColor Cyan
Write-Host ""

# Check auth
Write-Host "Checking authentication..." -ForegroundColor Yellow
& $GH auth status
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error: Not authenticated. Run: gh auth login" -ForegroundColor Red
    exit 1
}

# Add collaborators
Write-Host ""
Write-Host "Adding team members as collaborators..." -ForegroundColor Green

foreach ($member in $TEAM_MEMBERS) {
    $username = $member.Username
    $permission = $member.Permission
    
    Write-Host "  Adding: $username ($permission)" -ForegroundColor Yellow
    
    # Using GitHub API to add collaborator
    # Uncomment to execute:
    # & $GH api repos/$OWNER/$REPO/collaborators/$username -X PUT -F permission=$permission
    
    Write-Host "    Command: gh api repos/$OWNER/$REPO/collaborators/$username -X PUT -F permission=$permission" -ForegroundColor Gray
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Alternative: Use GitHub UI" -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "URL: https://github.com/$OWNER/$REPO/settings/access" -ForegroundColor Cyan
Write-Host ""
Write-Host "Steps:" -ForegroundColor Gray
Write-Host "1. Click 'Add people'" -ForegroundColor Gray
Write-Host "2. Search for GitHub username" -ForegroundColor Gray
Write-Host "3. Select permission level:" -ForegroundColor Gray
Write-Host "   - Read: View and clone only" -ForegroundColor Gray
Write-Host "   - Write (push): Push to branches" -ForegroundColor Gray
Write-Host "   - Admin: Full access" -ForegroundColor Gray
Write-Host "4. Click 'Add [username] to this repository'" -ForegroundColor Gray
Write-Host ""

Write-Host "Recommended permissions for EDA team:" -ForegroundColor Yellow
Write-Host "  - TechLead (eda_tech_lead): Admin" -ForegroundColor Gray
Write-Host "  - Developers (eda_cpp_dev, eda_qt_dev, eda_devops): Write (push)" -ForegroundColor Gray
Write-Host "  - QA (eda_qa): Write (push)" -ForegroundColor Gray
Write-Host ""

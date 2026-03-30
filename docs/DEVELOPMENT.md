# EDA Project - Development Guide

## Quick Start

### Prerequisites

- CMake 3.16+
- Qt 5.15.2+
- Visual Studio 2019+ (Windows) or GCC 9+ (Linux)

### Build

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build -j8

# Test
cd build && ctest --output-on-failure

# Run
./build/bin/EDA
```

## Project Structure

```
eda-app/
├── src/              # Source files
├── include/          # Header files
├── tests/            # Unit tests
├── docs/             # Documentation
├── cmake/            # CMake modules
├── packaging/        # Package configs
├── scripts/          # Build scripts
└── .github/          # GitHub workflows
```

## Git Workflow

### Branch Strategy

- `main` - Stable releases (protected)
- `dev` - Development branch
- `feature/*` - Feature branches

### Common Commands

```bash
# Create feature branch
git checkout -b feature/my-feature dev

# Commit changes
git add .
git commit -m "feat: add new feature"

# Push and create PR
git push -u origin feature/my-feature

# Sync with dev
git fetch origin
git rebase origin/dev
```

## Code Style

- C++17 standard
- Qt coding conventions
- Use `clang-format` before commit

## CI/CD

- **CI**: Runs on push to main/dev
- **Release**: Triggered by version tags (v*)
- **CodeQL**: Security scanning

## Testing

```bash
# Run all tests
ctest --output-on-failure

# Run specific test
ctest -R TestName --verbose

# With coverage (Linux)
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON ..
```

## Packaging

### Windows (NSIS)

```bash
cmake --build build --config Release
cpack -G NSIS
```

### Linux (DEB)

```bash
cmake -DCPACK_GENERATOR=DEB ..
cmake --build build
cpack -G DEB
```

## Troubleshooting

### CMake can't find Qt

```bash
# Set Qt directory
cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/5.15.2 ..
```

### Build fails on Windows

```bash
# Clean and rebuild
rm -rf build/
cmake -B build ..
cmake --build build
```

### Tests fail

```bash
# Run with verbose output
ctest --output-on-failure --verbose
```

## Documentation

- API Docs: `docs/api/`
- User Guide: `docs/user/`
- Dev Guide: `docs/dev/`

## Contact

- Issues: https://github.com/owner/eda-app/issues
- Discussions: https://github.com/owner/eda-app/discussions

---

_EDA Team - 2026_

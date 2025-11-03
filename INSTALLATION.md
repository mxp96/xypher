# Installation Guide

Complete installation instructions for all platforms.

## System Requirements

- CMake 3.20 or higher
- C++20 compatible compiler
- LLVM 14+ (LLVM 21 recommended)
- Clang (for linking)
- Git (optional)

## Platform-Specific Instructions

### Ubuntu/Debian Linux

```bash
sudo apt update
sudo apt install -y \
    llvm-21-dev \
    llvm-21 \
    clang \
    cmake \
    build-essential \
    zlib1g-dev \
    libzstd-dev \
    libxml2-dev \
    libedit-dev \
    libffi-dev \
    libtinfo-dev \
    git
```

**Alternative for LLVM 14:**

```bash
sudo apt install -y \
    llvm-14-dev \
    llvm-14 \
    clang \
    cmake \
    build-essential \
    zlib1g-dev \
    libzstd-dev \
    git
```

**Verify installation:**

```bash
llvm-config --version
cmake --version
clang --version
```

### Fedora/RHEL/CentOS

```bash
sudo dnf install -y \
    llvm-devel \
    clang \
    cmake \
    gcc-c++ \
    zlib-devel \
    libzstd-devel \
    libxml2-devel \
    libedit-devel \
    libffi-devel \
    ncurses-devel \
    git
```

### Arch Linux

```bash
sudo pacman -S \
    llvm \
    clang \
    cmake \
    base-devel \
    zlib \
    zstd \
    libxml2 \
    libedit \
    libffi \
    git
```

### macOS

**Using Homebrew:**

```bash
brew install llvm cmake git

export LLVM_DIR=$(brew --prefix llvm)/lib/cmake/llvm
export PATH="$(brew --prefix llvm)/bin:$PATH"
```

**Add to shell profile (~/.zshrc or ~/.bash_profile):**

```bash
echo 'export LLVM_DIR=$(brew --prefix llvm)/lib/cmake/llvm' >> ~/.zshrc
echo 'export PATH="$(brew --prefix llvm)/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

### Windows

**Using Chocolatey:**

```powershell
choco install llvm cmake visualstudio2022buildtools git
```

**Using Visual Studio:**

1. Install Visual Studio 2022 (Community/Professional/Enterprise)
2. Select "Desktop development with C++" workload
3. Install CMake from https://cmake.org/download/
4. Install LLVM from https://releases.llvm.org/

**Manual LLVM Installation:**

1. Download LLVM from https://github.com/llvm/llvm-project/releases
2. Extract to `C:\LLVM`
3. Add to PATH: `C:\LLVM\bin`
4. Set environment variable: `LLVM_DIR=C:\LLVM\lib\cmake\llvm`

**Using MSYS2 (MinGW):**

```bash
pacman -S mingw-w64-x86_64-llvm mingw-w64-x86_64-clang \
    mingw-w64-x86_64-cmake mingw-w64-x86_64-toolchain
```

## Verification

After installation, verify all components:

```bash
llvm-config --version
cmake --version
clang --version
g++ --version
```

Expected output:
```
LLVM version X.X.X
cmake version 3.XX.X
clang version X.X.X
g++ (GCC) X.X.X
```

## Troubleshooting

### LLVM Not Found

**Linux/macOS:**

```bash
export LLVM_DIR=/usr/lib/llvm-21/lib/cmake/llvm

llvm-config --cmakedir
```

**Windows:**

```powershell
$env:LLVM_DIR="C:\LLVM\lib\cmake\llvm"
```

### CMake Version Too Old

**Ubuntu/Debian:**

```bash
wget https://github.com/Kitware/CMake/releases/download/v3.27.0/cmake-3.27.0-linux-x86_64.sh
sudo bash cmake-3.27.0-linux-x86_64.sh --prefix=/usr/local --skip-license
```

**Using pip:**

```bash
pip install cmake --upgrade
```

### Compiler Not Found

**Linux:**

```bash
sudo apt install build-essential
```

**macOS:**

```bash
xcode-select --install
```

### Missing Dependencies

**If build fails with missing libraries:**

```bash
sudo apt install -y \
    libncurses5-dev \
    libtinfo-dev \
    libxml2-dev \
    libedit-dev \
    libffi-dev
```

## Next Steps

After successful installation, proceed to:

1. [Building](BUILDING.md) - Build the compiler
2. [Quickstart](QUICKSTART.md) - First program
3. [README](README.md) - Project overview

## Platform Notes

### Linux

- LLVM 21 is recommended for latest features
- LLVM 14 is stable and widely available
- Use system package manager for consistency

### macOS

- Homebrew LLVM is separate from Xcode LLVM
- Always use Homebrew LLVM for development
- Set LLVM_DIR environment variable

### Windows

- Visual Studio 2022 recommended
- MSYS2 provides Unix-like environment
- PowerShell or CMD both supported
- Path with spaces may cause issues

## Minimal Installation

For minimal setup (development only):

```bash
sudo apt install llvm-21-dev clang cmake build-essential
```

This provides basic functionality without optional features.

## Docker Alternative

Skip installation entirely using Docker:

```bash
docker-compose up -d
docker-compose exec xypher bash
```

See [docker/README.md](docker/README.md) for details.

